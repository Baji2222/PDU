# 8367 rack firmware - FIXED (ping timeout when web page opens)

## Folder map
| Folder | What it is |
|---|---|
| `01_CHANGED_FILES/` | ONLY the 6 files I modified, same sub-paths as the project. Quick drop-in. |
| `02_DIFFS/` | One `.diff` per changed file + `original_files/` (untouched originals). |
| `03_FULL_PROJECT/` | Complete project (`common/` + `rtl8367c/`) with all fixes applied. Open the same `.uvproj` as before (`rtl8367c/a_set/lws_a_set_*.uvproj`). |
| `04_PIC_firmware/` | `8367_fmd.c` (PIC side) - unchanged, copied from the original zip. |

## Root cause
`WEB_PRINTF` = `http_respPrintf` -> `vsprintf()` into `http_bufPrintf[256]` (no size limit).
The new PDU page (`get_pdu()`) printed 5456 / 4200 / 1181 / 413-byte strings in ONE call each,
overflowing RAM next to the TCP/IP state -> switch hangs -> ping "Request timed out".

## Changes (6 files)
| # | File | Fix |
|---|---|---|
| 1 | `common/include/web/web_api.h` | Added `WEB_PUTS(pReq,s)` (no vsprintf, no size limit, no `%%` escaping) + `#include <string.h>` |
| 2 | `rtl8367c/src/web/web_syslogbuf.c` `get_pdu()` | Static text -> `WEB_PUTS`. Port template split into 10 small calls (<256 B each). Also fixes: template had 14 `%d` but only 12 args; CSS had 8 bare `%` inside a printf format. |
| 3 | `rtl8367c/src/web/web_qos.c` `get_rtc()` | Same overflow (4046-byte single call, also in your old working bin). -> `WEB_PUTS`, `%%` -> `%`. |
| 4 | `rtl8367c/src/web/web_user.c` + `common/src/app/snmp/msg_out.c` | `Get_Response[75]` -> `[200]` (UART line up to 180 chars) and unbounded `strcpy` -> bounded `strncpy`. Both declarations match. |
| 5 | `rtl8367c/src/web/web_syslogbuf.c` + `html_api.c` | Added missing routes `/get_pdu_names` (new handler, returns 8 empty names) and `/set_pdu_name` (existing handler). |

## Verified (no Keil in my environment)
Host-compiled test of `get_pdu()`, `get_pdu_names()`, `get_rtc()`: largest `WEB_PRINTF` = 157 bytes (was 5456),
0 overflowing calls, outlets 1-8 have correct ids, CSS `%` intact. Re-scan of all 1921 `WEB_PRINTF` calls: 0 over 200 bytes.

## NOT done - you must do
* **Rebuild in Keil uVision** (I could not compile with C51) and upload the new bin. The `.bin` files in the tree are the OLD builds.
* Outlet names are NOT saved (the storage code was already commented out in the original). Page loads fine, names stay empty.

## Left untouched on purpose (not the cause; optional later)
* `main.c`: gateway ACL call uses `netmask = 0` (wildcard DIP).
* `firmware_upgrade.c` `upgrade_write_img()`: `ret` not assigned from `FLASH_WRITE`, bare `return;` in an `int16` function.
