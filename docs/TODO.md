# esp32claw vs Nanobot — Feature Gap Tracker

> Comparing against `nanobot/` reference implementation. Tracks features esp32claw has not yet aligned with.
> Priority: P0 = Core missing, P1 = Important enhancement, P2 = Nice to have

---

## P0 — Core Agent Capabilities

### [x] ~~Tool Use Loop (multi-turn agent iteration)~~
- Implemented: `agent_loop.c` ReAct loop with `llm_chat_tools()`, max 10 iterations, non-streaming JSON parsing

### [x] ~~Memory Write via Tool Use (agent-driven memory persistence)~~
- Implemented through `read_file` / `write_file` / `edit_file`
- `context_builder.c` instructs the agent to persist important facts into `MEMORY.md` and daily note files

### [x] ~~Tool Registry + web_search Tool~~
- Implemented: `tools/tool_registry.c` — tool registration, JSON schema builder, dispatch by name
- Implemented: `tools/tool_web_search.c` — Brave Search API via HTTPS (direct + proxy support)

### [~] More Built-in Tools
- Implemented: `read_file`, `write_file`, `edit_file`, `list_dir`, `get_current_time`, `get_device_status`, `cron_*`, `gpio_*`, `led_*`
- Remaining obvious gap from this list: a dedicated outbound `message` tool

### [ ] Subagent / Spawn Background Tasks
- **nanobot**: `subagent.py` — SubagentManager spawns independent agent instances with isolated tool sets and system prompts, announces results back to main agent via system channel
- **esp32claw**: Not implemented
- **Recommendation**: ESP32 memory is limited; simplify to a single background FreeRTOS task for long-running work, inject result into inbound queue on completion

---

## P1 — Important Features

### [ ] Telegram User Allowlist (allow_from)
- **nanobot**: `channels/base.py` L59-82 — `is_allowed()` checks sender_id against allow_list
- **esp32claw**: No authentication; anyone can message the bot and consume API credits
- **Recommendation**: Store allow_from list in `mimi_secrets.h` as a build-time define, filter in `process_updates()`

### [ ] Telegram Markdown to HTML Conversion
- **nanobot**: `channels/telegram.py` L16-76 — `_markdown_to_telegram_html()` full converter: code blocks, inline code, bold, italic, links, strikethrough, lists
- **esp32claw**: Uses `parse_mode: Markdown` directly; special characters can cause send failures (has fallback to plain text)
- **Recommendation**: Implement simplified Markdown-to-HTML converter, or switch to `parse_mode: HTML`

### [ ] Telegram /start Command
- **nanobot**: `telegram.py` L183-192 — handles `/start` command, replies with welcome message
- **esp32claw**: Not handled; /start is sent to Claude as a regular message

### [ ] Telegram Media Handling (photos/voice/files)
- **nanobot**: `telegram.py` L194-289 — handles photo, voice, audio, document; downloads files; transcribes voice
- **esp32claw**: Only processes `message.text`, ignores all media messages
- **Recommendation**: Images can be base64-encoded for Claude Vision; voice requires Whisper API (extra HTTPS request)

### [x] ~~Skills System (pluggable capabilities)~~
- Implemented in simplified form via SPIFFS markdown skills and `skill_loader.c`
- Current implementation focuses on discovery + prompt summarization rather than full metadata/frontmatter semantics

### [ ] Full Bootstrap File Alignment
- **nanobot**: Loads `AGENTS.md`, `SOUL.md`, `USER.md`, `TOOLS.md`, `IDENTITY.md` (5 files)
- **esp32claw**: Only loads `SOUL.md` and `USER.md`
- **Recommendation**: Add AGENTS.md (behavior guidelines) and TOOLS.md (tool documentation)

### [ ] Longer Memory Lookback
- **nanobot**: `memory.py` L56-80 — `get_recent_memories(days=7)` defaults to 7 days
- **esp32claw**: `context_builder.c` only reads last 3 days
- **Recommendation**: Make configurable, but mind token budget

### [x] ~~System Prompt Tool Guidance~~
- Implemented: `context_builder.c` includes tool usage guidance in system prompt

### [ ] Message Metadata (media, reply_to, metadata)
- **nanobot**: `bus/events.py` — InboundMessage has media, metadata fields; OutboundMessage has reply_to
- **esp32claw**: `mimi_msg_t` only has channel + chat_id + content
- **Recommendation**: Extend msg struct, add media_path and metadata fields

### [ ] Outbound Subscription Pattern
- **nanobot**: `bus/queue.py` L41-49 — supports `subscribe_outbound(channel, callback)` subscription model
- **esp32claw**: Hardcoded if-else dispatch
- **Recommendation**: Current approach is simple and reliable; not worth changing with few channels

---

## P2 — Advanced Features

### [x] ~~Cron Scheduled Task Service~~
- Implemented in `cron_service.c`
- Current scope supports recurring `every` jobs and one-shot `at` jobs with SPIFFS persistence

### [x] ~~Heartbeat Service~~
- Implemented in `heartbeat.c`
- Periodically scans `HEARTBEAT.md` and injects an agent turn when actionable items exist

### [x] ~~Multi-LLM Provider Support~~
- Implemented at a practical level in `llm_proxy.c`
- Current runtime supports `anthropic` plus OpenAI-compatible providers via `set_model_provider`

### [ ] Voice Transcription
- **nanobot**: `providers/transcription.py` — Groq Whisper API
- **esp32claw**: Not implemented
- **Recommendation**: Requires extra HTTPS request to Whisper API: download Telegram voice -> forward -> get text

### [x] ~~Build-time Config File + Runtime NVS Override~~
- Implemented: `mimi_secrets.h` as build-time defaults, NVS as runtime override via CLI
- Two-layer config: build-time secrets → NVS fallback, CLI commands to set/show/reset

### [ ] WebSocket Gateway Protocol Enhancement
- **nanobot**: Gateway port 18790 + richer protocol
- **esp32claw**: Basic JSON protocol, lacks streaming token push
- **Recommendation**: Add `{"type":"token","content":"..."}` streaming push

### [ ] Multi-Channel Manager
- **nanobot**: `channels/manager.py` — unified lifecycle management for multiple channels
- **esp32claw**: Hardcoded in app_main()
- **Recommendation**: Not worth abstracting with few channels

### [~] WhatsApp / Feishu Channels
- Feishu is implemented in `channels/feishu/`
- WhatsApp is still not implemented

### [x] ~~Telegram Proxy Support (HTTP CONNECT)~~
- Implemented: HTTP CONNECT tunnel via `proxy/http_proxy.c`, configurable via `mimi_secrets.h` (`MIMI_SECRET_PROXY_HOST`/`MIMI_SECRET_PROXY_PORT`)

### [ ] Session Metadata Persistence
- **nanobot**: `session/manager.py` L136-153 — session file includes metadata line (created_at, updated_at)
- **esp32claw**: JSONL only stores role/content/ts, no metadata header
- **Recommendation**: Low priority

---

## Completed Alignment

- [x] Telegram Bot long polling (getUpdates)
- [x] Message Bus (inbound/outbound queues)
- [x] Agent Loop with ReAct tool use (multi-turn, max 10 iterations)
- [x] LLM API integration (Anthropic + OpenAI-compatible, non-streaming, tool_use protocol)
- [x] Tool Registry + web_search tool (Brave Search API)
- [x] Context Builder (system prompt + bootstrap files + memory + tool guidance)
- [x] Memory Store (MEMORY.md + daily notes)
- [x] Session Manager (JSONL per chat_id, ring buffer history)
- [x] WebSocket Gateway (port 18789, JSON protocol)
- [x] Feishu channel
- [x] Serial CLI (esp_console, debug/maintenance commands)
- [x] HTTP CONNECT Proxy (Telegram + LLM APIs + Brave Search via proxy tunnel)
- [x] SOCKS5 proxy support
- [x] Skills loader
- [x] Cron scheduler
- [x] Heartbeat service
- [ ] OTA startup/build wiring
- [x] WiFi Manager (build-time credentials, exponential backoff)
- [x] SPIFFS storage
- [x] Build-time config (`mimi_secrets.h`) + runtime NVS override via CLI

---

## Suggested Implementation Order

```
1. Telegram Allowlist (allow_from)   <- security essential
2. Message metadata and richer session naming
3. Bootstrap File Completion (AGENTS.md, TOOLS.md)
4. Dedicated outbound `message` tool
5. Telegram Markdown -> HTML
6. Media Handling
7. OTA wiring into the active build/startup path
8. Other enhancements
```

