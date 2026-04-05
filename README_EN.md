# Esp32Claw: Deploy an AI Assistant on ESP32-S3


<p align="left">
  <strong><a href="README_EN.md">English</a> | <a href="README.md">中文</a> </strong>
</p>

**This project is based on [MimiClaw](https://github.com/memovai/mimiclaw). It deploys a local AI assistant on an ESP32-S3 development board costing around 25 RMB, and supports features such as controlling GPIO through Feishu, reading and writing local files, and driving WS2812B LEDs for breathing light effects.**

## Meet [MimiClaw](https://github.com/memovai/mimiclaw)

- **Tiny** — No Linux, no Node.js, no bloated dependencies — pure C
- **Easy to use** — Just send a message on Telegram, and it handles the rest
- **Loyal** — Learns from memory and remembers across reboots
- **Capable** — USB powered, 0.5W, runs 24/7
- **Cute** — One ESP32-S3 development board, about $5, that’s it

## Compared with MimiClaw, esp32Claw includes the following changes:

- **Uses the MiniMax API, starting from 29 RMB per month**
- **Adds WS2812B support for breathing light effects**
- **Adds a device status query tool, allowing you to check Wi-Fi, IP address, uptime, memory, PSRAM, current light mode, color, brightness, and breathing cycle through Feishu**
- **Improves the Wi-Fi reconnection logic to avoid crashes**
- **Fixes the issue where the Telegram branch still runs even when only Feishu is configured**
- **Optimizes the local management AP so it no longer stays active after Wi-Fi connects, saving resources**

## Quick Start

### What you need

- An **ESP32-S3 development board**, 16MB Flash + 8MB PSRAM
- A **USB Type-C data cable**
- A **Feishu bot**
- A **large-model API**

### Ubuntu and macOS installation

Please refer to [MimiClaw](https://github.com/memovai/mimiclaw).

### Windows 11 installation

```bash
# Install ESP-IDF v5.5+ first:
# If git clone fails, download the v5.5.2 zip package from the official website
git clone -b v5.5.2 --recursive https://github.com/espressif/esp-idf.git

git clone https://github.com/zhangjun2636808827/esp32claw

# Install ESP-IDF first
cd esp-idf-v5.5.2
# Open cmd in the terminal, otherwise idf.py may not be recognized after installation
cmd
# Install
install.bat
export.bat
# Verify installation
idf.py --help
# If you see the expected help output, ESP-IDF has been installed successfully in this terminal
```

<img src="assets/install.png" alt="ESP-IDF installation" width="480" />

```bash
# Enter the esp32claw directory
cd esp32claw
# Select the target chip
idf.py set-target esp32s3
# Copy the configuration file; all information should be filled in this file
cp main/mimi_secrets.h.example main/mimi_secrets.h
```

<img src="assets/config.png" alt="Project configuration" width="480" />

### Get your [Feishu](https://open.feishu.cn/app) credentials

<img src="assets/feishu.png" alt="Feishu setup step 1" width="480" />
<img src="assets/feishu2.png" alt="Feishu setup step 2" width="480" />

### Copy App ID and App Secret into `MIMI_SECRET_FEISHU_APP_ID` and `MIMI_SECRET_FEISHU_APP_SECRET`

<img src="assets/feishu3.png" alt="Feishu app credentials" width="480" />

### Create a bot

<img src="assets/feishu4.png" alt="Create Feishu bot" width="480" />

### Configure bot permissions

<img src="assets/feishu5.png" alt="Configure Feishu bot permissions" width="480" />

```json
{
  "scopes": {
    "tenant": [
      "aily:file:read",
      "aily:file:write",
      "application:application.app_message_stats.overview:readonly",
      "application:application:self_manage",
      "application:bot.menu:write",
      "cardkit:card:read",
      "cardkit:card:write",
      "contact:user.employee_id:readonly",
      "corehr:file:download",
      "event:ip_list",
      "im:chat.access_event.bot_p2p_chat:read",
      "im:chat.members:bot_access",
      "im:message",
      "im:message.group_at_msg:readonly",
      "im:message.p2p_msg:readonly",
      "im:message:readonly",
      "im:message:send_as_bot",
      "im:resource"
    ],
    "user": [
      "aily:file:read",
      "aily:file:write",
      "im:chat.access_event.bot_p2p_chat:read"
    ]
  }
}
```

<img src="assets/feishu6.png" alt="Feishu permission JSON" width="480" />

### Enable the following permissions

<img src="assets/feishu7.png" alt="Enable Feishu permissions" width="480" />

### Configure event subscriptions

<img src="assets/feishu8.png" alt="Configure event subscriptions" width="480" />

### Publish the app

<img src="assets/feishu9.png" alt="Publish Feishu app step 1" width="480" />
<img src="assets/feishu10.png" alt="Publish Feishu app step 2" width="480" />

### Create a group chat in Feishu, then go to Settings -> Group Bots -> Add -> Select the bot you created earlier -> Mention your bot in the group and send a message, for example:

`@esp32claw Turn on a blue breathing light with a 2s cycle`

<img src="assets/feishu11.png" alt="Use the bot in a Feishu group" width="480" />

### Feishu setup is now complete. Next, configure your [MiniMax](https://www.minimaxi.com/) credentials

### Subscribe to the 29 RMB Starter plan on [MiniMax](https://www.minimaxi.com/)

### Go to Account Management -> Token Plan -> Copy the Token Plan Key into `MIMI_SECRET_API_KEY`

<img src="assets/MiniMax1.png" alt="MiniMax token plan key" width="480" />

```c
#define MIMI_SECRET_MODEL           "MiniMax-M2.7" // Model name, see https://minimaxi.com/docs/models
#define MIMI_SECRET_MODEL_PROVIDER  "openai"       // Provider; default is Anthropic. If using MiniMax models, set this to "openai"
```

### At this point, all required fields in `mimi_secrets.h` should be filled in

### Configure WS2812B by opening `mimiclaw-main/main/mimi_config.h`

<img src="assets/image8.png" alt="WS2812B configuration" width="480" />

### Next, build and flash the firmware

### Connect the ESP32 development board to your computer, and make sure to use the flashing port

<img src="assets/image.png" alt="Connect ESP32 board" width="480" />

### Open Device Manager

<img src="assets/image2.png" alt="Open Device Manager" width="480" />

### Find which COM port your board is connected to

<img src="assets/image3.png" alt="Find COM port" width="480" />

### In this example, the port is `COM5`. Now build the firmware

```bash
# Full rebuild is required after modifying mimi_secrets.h
idf.py fullclean && idf.py build

idf.py -p COM5 flash monitor
```

<img src="assets/image4.png" alt="Build output" width="480" />
<img src="assets/image5.png" alt="Flash output" width="480" />

### If you see the following, compilation and flashing were successful

<img src="assets/image6.png" alt="Successful build and flash 1" width="480" />
<img src="assets/image7.png" alt="Successful build and flash 2" width="480" />

### Connect the USB cable to the serial port, open MobaXterm (`MobaXterm/MobaXterm.exe`), and follow the steps below to open the serial port

<img src="assets/image9.png" alt="Open serial port in MobaXterm step 1" width="480" />
<img src="assets/image10.png" alt="Open serial port in MobaXterm step 2" width="480" />

### Press the reset button on the ESP32 development board

<img src="assets/image11.png" alt="Reset ESP32 board" width="480" />

### Now start chatting!

<img src="assets/image12.jpg" alt="Chat example 1" width="480" />
<img src="assets/image13.jpg" alt="Chat example 2" width="480" />
<img src="assets/image14.png" alt="Chat example 3" width="480" />

### CLI Commands (via UART/COM port)

You can configure and debug the device through the serial port. **Configuration commands** let you change settings without recompiling — just plug in a USB cable anytime and update them.

**Runtime configuration** (stored in NVS and overrides compile-time defaults):

```text
mimi> wifi_set MySSID MyPassword   # Change Wi-Fi
mimi> set_tg_token 123456:ABC...   # Change Telegram Bot Token
mimi> set_api_key sk-ant-api03-... # Change API Key (Anthropic or OpenAI)
mimi> set_model_provider openai    # Switch provider (anthropic|openai)
mimi> set_model gpt-4o             # Change model
mimi> set_proxy 192.168.1.83 7897  # Set proxy
mimi> clear_proxy                  # Clear proxy
mimi> set_search_key BSA...        # Set Brave Search API Key
mimi> set_tavily_key tvly-...      # Set Tavily API Key (preferred)
mimi> config_show                  # Show all configuration (masked)
mimi> config_reset                 # Clear NVS and restore compile-time defaults
```

**Debugging and maintenance:**

```text
mimi> wifi_status              # Is it connected?
mimi> memory_read              # Check what it remembers
mimi> memory_write "content"   # Write to MEMORY.md
mimi> heap_info                # Check remaining memory
mimi> session_list             # List all sessions
mimi> session_clear 12345      # Delete a session
mimi> heartbeat_trigger        # Manually trigger a heartbeat check
mimi> cron_start               # Start the cron scheduler immediately
mimi> restart                  # Reboot
```

## Memory

MimiClaw stores all data as plain text files, which can be read and edited directly:

| File | Description |
|------|-------------|
| `SOUL.md` | The bot's personality — edit it to change behavior |
| `USER.md` | Information about you — name, preferences, language |
| `MEMORY.md` | Long-term memory — things it should always remember |
| `HEARTBEAT.md` | To-do list — the bot checks and executes tasks periodically |
| `cron.json` | Scheduled tasks — recurring or one-time tasks created by the AI |
| `2026-02-05.md` | Daily note — what happened today |
| `tg_12345.jsonl` | Chat history — your conversations with it |

## Tools

MimiClaw supports tool calling for both Anthropic and OpenAI. The LLM can call tools during a conversation and loop until the task is completed (ReAct mode).

| Tool | Description |
|------|-------------|
| `web_search` | Search the web via Tavily (preferred) or Brave for real-time information |
| `get_current_time` | Fetch the current date and time via HTTP and set the system clock |
| `cron_add` | Create scheduled or one-time tasks (the LLM can create cron jobs autonomously) |
| `cron_list` | List all scheduled cron tasks |
| `cron_remove` | Remove a cron task by ID |

To enable web search, set a [Tavily API key](https://app.tavily.com/home) (`MIMI_SECRET_TAVILY_KEY`, preferred) or a [Brave Search API key](https://brave.com/search/api/) (`MIMI_SECRET_SEARCH_KEY`) in `mimi_secrets.h`.

## Scheduled Tasks (Cron)

MimiClaw includes a built-in cron scheduler, allowing the AI to schedule tasks autonomously. The LLM can use the `cron_add` tool to create recurring tasks ("every N seconds") or one-time tasks ("at a specific timestamp"). When triggered, the message is injected into the agent loop — the AI wakes up automatically, processes the task, and replies.

Tasks are stored persistently in SPIFFS (`cron.json`) and will survive reboots. Typical use cases include daily summaries, reminders, and regular inspections.

## Heartbeat

The heartbeat service periodically reads `HEARTBEAT.md` from SPIFFS and checks whether there are pending tasks. If it finds unfinished items (non-empty lines, non-title lines, and unchecked entries instead of `- [x]`), it sends a prompt to the agent loop so the AI can process them autonomously.

This turns MimiClaw into a proactive assistant — write tasks into `HEARTBEAT.md`, and the bot will automatically pick them up during the next heartbeat cycle (default: every 30 minutes).

## Other Features

- **WebSocket gateway** — Port 18789, connect from any WebSocket client within the local network
- **OTA updates** — Flash firmware over Wi-Fi without USB
- **Dual-core** — Network I/O and AI processing run on separate CPU cores
- **HTTP proxy** — CONNECT tunneling for restricted network environments
- **Multi-provider** — Supports both Anthropic (Claude) and OpenAI (GPT), switchable at runtime
- **Scheduled tasks** — The AI can create recurring and one-time tasks autonomously, with persistence across reboots
- **Heartbeat service** — Periodically checks task files and drives autonomous execution
- **Tool calling** — ReAct agent loop with tool support for both providers

## License

MIT

## Acknowledgements

Thanks to the developers of MimiClaw.

Visit the [MimiClaw](https://github.com/memovai/mimiclaw) homepage.
