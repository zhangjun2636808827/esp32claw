# esp32claw

<div align="center">

### A lightweight, always-on AI assistant for ESP32-S3

<p>
  <strong><a href="README.md">中文</a></strong> ·
  <strong><a href="README_EN.md">English</a></strong>
</p>

</div>

> A fork and extension of the upstream [MimiClaw](https://github.com/memovai/mimiclaw).  
> `esp32claw` runs on a low-cost ESP32-S3 development board and supports Feishu, Telegram, WebSocket, tool execution, GPIO control, scheduled tasks, heartbeat wakeups, SPIFFS-based memory, and WS2812B breathing light effects.

## Highlights

| Feature | Description |
|---------|-------------|
| Lightweight runtime | No Linux, no Node.js, pure C |
| Multi-channel chat | Feishu, Telegram, and WebSocket |
| Tool execution | Files, GPIO, LED, web search, time, device status, and cron |
| Persistent memory | Sessions, memory, skills, and tasks stored in SPIFFS |
| Low power | Suitable for USB or battery-powered deployments |
| Fun to build | A full device-side AI agent on a single ESP32-S3 board |

## Meet esp32claw

- **Tiny**: no Linux, no Node.js, no bloated dependency stack
- **Action-oriented**: send a message and it can call tools to do real work
- **Persistent**: memory, sessions, and tasks survive reboots
- **Always on**: low cost and low power, suitable for 24/7 runtime
- **Playful**: chat, LEDs, file operations, and automation in one board

## What changed from upstream MimiClaw

- **MiniMax API integration** for lower daily usage cost
- **WS2812B driver support** with breathing light effects
- **Device status tool** for Wi-Fi, IP, uptime, memory, PSRAM, and LED state
- **Improved Wi-Fi reconnection logic** to reduce freeze/crash risk
- **Feishu-only deployments no longer keep Telegram logic active**
- **Local admin AP behavior optimized** to reduce runtime overhead after Wi-Fi connects

## Quick Navigation

- [Quick Start](#quick-start)
- [CLI Commands](#cli-commands-via-uartcom-port)
- [Memory](#memory)
- [Tools](#tools)
- [Scheduled Tasks (Cron)](#scheduled-tasks-cron)
- [Heartbeat](#heartbeat)
- [Other Features](#other-features)
- [License](#license)

## Quick Start

### What you need

- An **ESP32-S3 development board**, 16MB Flash + 8MB PSRAM
- A **USB Type-C data cable**
- A **Feishu bot**
- A **large-model API**

### Ubuntu and macOS

Please refer to the upstream [MimiClaw](https://github.com/memovai/mimiclaw).

### Windows 11

```bash
# Install ESP-IDF v5.5+ first
# If git clone fails, download the v5.5.2 zip package from the official website
git clone -b v5.5.2 --recursive https://github.com/espressif/esp-idf.git

git clone https://github.com/zhangjun2636808827/esp32claw

# Install ESP-IDF first
cd esp-idf-v5.5.2

# Open cmd in the terminal, otherwise idf.py may not be recognized after install
cmd

# Install
install.bat
export.bat

# Verify installation
idf.py --help
```

<img src="assets/install.png" alt="ESP-IDF installation" width="1080" />

```bash
# Enter the esp32claw directory
cd esp32claw

# Select the target chip
idf.py set-target esp32s3

# Copy the configuration file; all required information should be filled in here
cp main/mimi_secrets.h.example main/mimi_secrets.h
```

<img src="assets/config.png" alt="Project configuration" width="1080" />

### Get your [Feishu](https://open.feishu.cn/app) credentials

<img src="assets/feishu.png" alt="Feishu setup step 1" width="1080" />
<img src="assets/feishu2.png" alt="Feishu setup step 2" width="1080" />

### Copy App ID and App Secret into `MIMI_SECRET_FEISHU_APP_ID` and `MIMI_SECRET_FEISHU_APP_SECRET`

<img src="assets/feishu3.png" alt="Feishu app credentials" width="1080" />

### Create a bot

<img src="assets/feishu4.png" alt="Create Feishu bot" width="1080" />

### Configure bot permissions

<img src="assets/feishu5.png" alt="Configure Feishu bot permissions" width="1080" />

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

<img src="assets/feishu6.png" alt="Feishu permission JSON" width="1080" />

### Enable the following permissions

<img src="assets/feishu7.png" alt="Enable Feishu permissions" width="1080" />

### Configure event subscriptions

<img src="assets/feishu8.png" alt="Configure event subscriptions" width="1080" />

### Publish the app

<img src="assets/feishu9.png" alt="Publish Feishu app step 1" width="1080" />
<img src="assets/feishu10.png" alt="Publish Feishu app step 2" width="1080" />

### Test the bot in Feishu

Create a group chat in Feishu, then go to `Settings -> Group Bots -> Add`, select the bot you created earlier, and send a message such as:

`@esp32claw Turn on a blue breathing light with a 2s cycle`

<img src="assets/feishu11.png" alt="Use the bot in a Feishu group" width="240" />

### Configure your [MiniMax](https://www.minimaxi.com/) credentials

- Subscribe to the 29 RMB Starter plan on [MiniMax](https://www.minimaxi.com/)
- Go to `Account Management -> Token Plan`
- Copy the Token Plan Key into `MIMI_SECRET_API_KEY`

<img src="assets/MiniMax1.png" alt="MiniMax token plan key" width="1080" />

```c
#define MIMI_SECRET_MODEL           "MiniMax-M2.7" // Model name, see https://minimaxi.com/docs/models
#define MIMI_SECRET_MODEL_PROVIDER  "openai"       // Provider; default is Anthropic. If using MiniMax models, set this to "openai"
```

### After `mimi_secrets.h` is filled in, configure WS2812B

Open `main/mimi_config.h`:

<img src="assets/image8.png" alt="WS2812B configuration" width="1080" />

### Build and flash the firmware

### Connect the ESP32 development board to your computer and make sure to use the flashing port

<img src="assets/image.png" alt="Connect ESP32 board" width="1080" />

### Open Device Manager

<img src="assets/image2.png" alt="Open Device Manager" width="240" />

### Find which COM port the board is connected to

<img src="assets/image3.png" alt="Find COM port" width="1080" />

### In this example, the port is `COM5`

```bash
# A full rebuild is recommended after modifying mimi_secrets.h
idf.py fullclean && idf.py build

idf.py -p COM5 flash monitor
```

<img src="assets/image4.png" alt="Build output" width="1080" />
<img src="assets/image5.png" alt="Flash output" width="1080" />

### If you see the following, compilation and flashing were successful

<img src="assets/image6.png" alt="Successful build and flash 1" width="1080" />
<img src="assets/image7.png" alt="Successful build and flash 2" width="1080" />

### Connect the serial port and open MobaXterm

Connect the USB cable to the serial port, open `MobaXterm/MobaXterm.exe`, and follow the steps below:

<img src="assets/image9.png" alt="Open serial port in MobaXterm step 1" width="1080" />
<img src="assets/image10.png" alt="Open serial port in MobaXterm step 2" width="1080" />

### Press the reset button on the ESP32 board

<img src="assets/image11.png" alt="Reset ESP32 board" width="1080" />

### Now start chatting

<img src="assets/image12.jpg" alt="Chat example 1" width="240" />
<img src="assets/image13.jpg" alt="Chat example 2" width="240" />
<img src="assets/image14.png" alt="Chat example 3" width="480" />

### CLI Commands (via UART/COM port)

You can configure and debug the device through the serial port. **Configuration commands** let you update settings without recompiling.

**Runtime configuration** stored in NVS and overriding compile-time defaults:

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

esp32claw stores all data as plain text files, which can be read and edited directly:

| File | Description |
|------|-------------|
| `SOUL.md` | The bot personality, editable to change behavior |
| `USER.md` | Information about you, such as name, preferences, and language |
| `MEMORY.md` | Long-term memory, things it should keep forever |
| `HEARTBEAT.md` | To-do list checked periodically by the bot |
| `cron.json` | Scheduled tasks created by the AI |
| `2026-02-05.md` | Daily note stored under `/spiffs/memory/` |
| `tg_12345.jsonl` | Chat history currently stored as `tg_<chat_id>.jsonl` under `/spiffs/sessions/` |

## Tools

esp32claw supports tool calling for both Anthropic and OpenAI. During a conversation, the LLM can call tools repeatedly until the task is completed, following a ReAct-style loop.

| Tool | Description |
|------|-------------|
| `web_search` | Search the web via Tavily (preferred) or Brave for real-time information |
| `get_current_time` | Fetch the current date and time via HTTP and set the system clock |
| `get_device_status` | Report Wi-Fi, IP, uptime, memory, PSRAM, and LED status |
| `read_file` / `write_file` / `edit_file` / `list_dir` | Read and modify files in SPIFFS |
| `cron_add` | Create scheduled or one-time tasks autonomously |
| `cron_list` | List all scheduled cron tasks |
| `cron_remove` | Remove a cron task by ID |
| `gpio_write` / `gpio_read` / `gpio_read_all` | Control and inspect allowed GPIO pins |
| `set_led_color` / `led_off` / `breathing_led_on` / `breathing_led_off` / `breathing_led_status` | Control the WS2812B LED on GPIO48 |

To enable web search, set a [Tavily API key](https://app.tavily.com/home) (`MIMI_SECRET_TAVILY_KEY`, preferred) or a [Brave Search API key](https://brave.com/search/api/) (`MIMI_SECRET_SEARCH_KEY`) in `mimi_secrets.h`.

## Scheduled Tasks (Cron)

esp32claw includes a built-in cron scheduler, allowing the AI to schedule tasks autonomously. The LLM can use `cron_add` to create recurring jobs such as "every N seconds" or one-time jobs at a specific timestamp. When triggered, the message is injected into the agent loop and processed automatically.

Tasks are stored persistently in SPIFFS as `cron.json`, so they survive reboots. Typical use cases include reminders, daily summaries, and periodic inspections.

## Heartbeat

The heartbeat service periodically reads `HEARTBEAT.md` from SPIFFS and checks whether there are pending tasks. If it finds unfinished entries, such as non-empty lines that are not headings and not marked `- [x]`, it prompts the agent loop to handle them automatically.

This makes esp32claw a proactive assistant. Write tasks into `HEARTBEAT.md`, and the bot will pick them up during the next heartbeat cycle, which defaults to every 30 minutes.

## Other Features

- **WebSocket gateway**: port `18789`, accessible from any client on the local network
- **Dual-core runtime**: network I/O and AI processing run on separate CPU cores
- **HTTP proxy**: supports CONNECT tunneling for restricted environments
- **SOCKS5 proxy**: optional SOCKS5 tunneling support
- **Multi-provider**: switch between Anthropic (Claude) and OpenAI (GPT) at runtime
- **Scheduled tasks**: autonomous recurring and one-time tasks with persistence
- **Heartbeat service**: periodic task scanning and proactive execution
- **Tool calling**: ReAct agent loop with tool support across providers

Note: an OTA source module exists in `main/ota/`, but it is not currently wired into the active startup path or `main/CMakeLists.txt`.

## License

MIT

## Acknowledgements

Thanks to the developers of the upstream MimiClaw.  
Visit the upstream [MimiClaw](https://github.com/memovai/mimiclaw) homepage.
