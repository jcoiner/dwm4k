#include "wide_layouts.h"
#include "types.h"

#include <string.h>
#include <X11/Xutil.h>

#include <list>
#include <unordered_map>
#include <vector>

extern "C" {
    void resize(Client*, int, int, int, int, int);
    Client* nexttiled(Client*);
    Display* get_display();
    void detach(Client *c);
    void attach_after(Client* location,
                      Client* new_neighbor);
}

#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)

namespace {

void safe_resize(Client* client,
                 unsigned x,
                 unsigned y,
                 unsigned w,
                 unsigned h) {
  //fprintf(stderr, "Will resize(%u, %u, %u, %u)\n", x, y, w, h);
  resize(client, x, y, w, h, False);
}

unsigned min(unsigned x, unsigned y) {
  return x < y ? x : y;
}

// Lay out the next `tiles` clients into a column.
// Returns the next Client to be laid out in the next column.
//
Client* tcl_col(unsigned tiles,
                unsigned col_x,
                unsigned col_y,
                unsigned col_w,
                unsigned col_h,
                Client* client) {
  unsigned my = 0;            // accumulated Y offset
  for (unsigned idx = 0; idx < tiles; idx++) {
    const unsigned bdw = 2 * client->bw;   // border width

    // Don't let remaining_col_height overflow or become negative.
    unsigned remaining_col_height;
    if (my < col_h - 20) {
      remaining_col_height = col_h - my;
    } else {
      remaining_col_height = 20;
    }
    const unsigned height = remaining_col_height / (tiles - idx);
    safe_resize(client,
                col_x,
                col_y + my,
                col_w - bdw,
                height - bdw);
    my += HEIGHT(client);
    client = nexttiled(client->next);
    if (!client) return client;
  }
  return client;
}

}  // namespace

void
tcl(Monitor * m)
{
  Client * c;

  unsigned total_tiles;
  for (total_tiles = 0, c = nexttiled(m->clients); c;
       c = nexttiled(c->next), total_tiles++)
    ;
  if (total_tiles == 0)
    return;

  // Always lay out at least one master, and not more masters
  // than we have tiles.
  const unsigned nmaster =
      m->nmaster ? min(m->nmaster, total_tiles) : 1;
  const unsigned secondary_tiles = total_tiles - nmaster;

  c = nexttiled(m->clients);

  // mw == main width, which may be one or two colums.
  const unsigned mw = secondary_tiles ? m->mfact * m->ww : m->ww;
  const unsigned secondary_width = m->ww - mw;

  if (nmaster == 1) {
    // One big wide primary window
    c = tcl_col(1,
                // X coord
                m->wx + secondary_width,
                // Y coord
                m->wy,
                // Width
                mw,
                // Height
                m->wh,
                c);
    if (!c) return;
  } else {
    // Split the masters between center and right columns.

    // 2 is 1, 1
    // 3 is 1, 2
    // 4 is 1, 3
    // 5 is 2, 3
    // 6 is 2, 4
    // 7 is 3, 4
    // ... and center col never grows beyond 3.

    const unsigned center_tiles =
        nmaster < 5 ? 1 :
        nmaster < 7 ? 2 : 3;
    const unsigned right_side_tiles = nmaster - center_tiles;

    c = tcl_col(center_tiles,
                m->wx + secondary_width,
                m->wy,
                mw / 2,
                m->wh,
                c);
    if (!c) return;

    c = tcl_col(right_side_tiles,
                m->wx + secondary_width + (mw / 2),
                m->wy,
                mw / 2,
                m->wh,
                c);
    if (!c) return;
  }

  // Side pane(s).
  if (secondary_tiles) {
    tcl_col(secondary_tiles,
            m->wx,
            m->wy,
            secondary_width,
            m->wh,
            c);
  }
}

namespace {

bool IsWide(Client* c) {
  return c->prefer_wide;
}

}  // namespace

void wide_tile(Monitor * m) {
  Client* c;

  // Intuition:
  //  - browsers need to be wide, since webpages are broken.
  //  - editors need to be tall
  //  - terminals, TBD
  std::unordered_map<Client*, bool> wide;
  std::vector<Client*> primaries;
  std::vector<Client*> secondaries;

  // Running count of tiles
  unsigned total_tiles = 0;
  // Running count of tall (not wide) primary tiles
  unsigned tall_primaries = 0;

  for (c = nexttiled(m->clients); c; c = nexttiled(c->next)) {
    wide[c] = IsWide(c);

    // Try to get at least one primary:
    if ((primaries.size() < std::max(1, m->nmaster)) ||
        // Promote a secondary to primary if needed
        // to make the grid nice:
        ((tall_primaries & 0x1) && !wide[c])) {
      // Make this a primary.
      primaries.push_back(c);
      if (!wide[c]) {
        tall_primaries++;
      }
    } else {
      secondaries.push_back(c);
    }
    total_tiles++;
  }
  if (total_tiles == 0)
    return;

  auto wide_tile_pane = [wide](const std::vector<Client*>& tiles,
                               bool right_to_left,
                               unsigned pane_x, unsigned pane_y,
                               unsigned pane_w, unsigned pane_h) {
    // Each row is either a wide tile, or a pair of tall tiles.
    std::list<std::vector<Client*>> rows;
    std::vector<Client*>* half_tall_row = nullptr;

    for (Client* tile : tiles) {
      if ((!wide.at(tile)) && half_tall_row) {
        // Complete a row of tall tiles.
        half_tall_row->push_back(tile);
        half_tall_row = nullptr;
      } else {
        std::vector<Client*> row;
        row.push_back(tile);
        rows.emplace_back(std::move(row));

        if (!wide.at(tile)) {
          half_tall_row = &rows.back();
        }
      }
    }

    // Render each row
    unsigned my = 0;  // cumulative Y offset
    unsigned rows_done = 0;
    const unsigned rows_total = rows.size();
    for (const std::vector<Client*>& row : rows) {
      unsigned remaining_pane_height;
      if (my < pane_h - 20) {
        // Normal case
        remaining_pane_height = pane_h - my;
      } else {
        // Don't let remaining_pane_height overflow or become negative.
        remaining_pane_height = 20;
      }

      const unsigned height =
          remaining_pane_height / (rows_total - rows_done);

      if (row.size() == 1) {
        Client* client = row.front();
        const unsigned bdw = 2 * client->bw;   // border width
        safe_resize(client,
                    pane_x,
                    pane_y + my,
                    pane_w - bdw,
                    height - bdw);
        my += HEIGHT(client);
      } else {
        Client* client_0 = row[0];
        const unsigned bdw_0 = 2 * client_0->bw;   // border width
        safe_resize(client_0,
                    pane_x + (right_to_left ? 0 : pane_w / 2),
                    pane_y + my,
                    (pane_w / 2) - bdw_0,
                    height - bdw_0);

        Client* client_1 = row[1];
        const unsigned bdw_1 = 2 * client_1->bw;
        safe_resize(client_1,
                    pane_x + (right_to_left ? pane_w / 2 : 0),
                    pane_y + my,
                    (pane_w / 2) - bdw_1,
                    height - bdw_1);
        my += std::max(HEIGHT(client_0), HEIGHT(client_1));

        // Promote row[1] to immediately follow row[0] in the tile list,
        // so that the visual layout and logical tile order continue to match.
        detach(client_1);
        attach_after(client_0, client_1);
      }

      rows_done++;
    }
  };

  if (secondaries.empty()) {
    wide_tile_pane(primaries,
                   // Right-to-left:
                   true,
                   // X coord
                   m->wx,
                   // Y coord
                   m->wy,
                   // Width
                   m->ww,
                   // Height
                   m->wh);
  } else {
    const unsigned primary_w = secondaries.empty() ? m->ww : m->mfact * m->ww;
    const unsigned secondary_w = m->ww - primary_w;

    wide_tile_pane(primaries,
                   // Right-to-left:
                   // Keep the primary centered
                   false,
                   // X coord
                   m->wx,
                   // Y coord
                   m->wy,
                   // Width
                   primary_w,
                   // Height
                   m->wh);
    wide_tile_pane(secondaries,
                   // Right-to-left:
                   true,
                   // X
                   m->wx + primary_w,
                   // Y
                   m->wy,
                   // Width
                   secondary_w,
                   // Height
                   m->wh);
  }
}
