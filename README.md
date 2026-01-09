# EV & Charging Stations — C11 Starter V2 (Data Structures)

This V2 integrates CSV/JSON loaders wired in `Makefile` and `main.c`.
Build & run:
```bash
make
./ev_demo
```
Files:
- events.h/.c — tiny event stream
- slist.h/.c — MRU SList (head-only)
- queue.h/.c — FIFO of Event
- stack.h/.c — stack for postfix rules
- station_index.h/.c — AVL stations index
- nary.h/.c — n-ary tree (skeleton + BFS print)
- rules.c — postfix evaluator (example)
- **csv_loader.h/.c** — load stations from CSV (IRVE-like)
- **json_loader.h/.c** — load stations from JSON (minimal format)
- main.c — demo: load CSV/JSON → ingest events → show AVL/MRU
