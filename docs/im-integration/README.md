# IM Integration Guides

Configuration guides for esp32claw's instant messaging channel integrations.

## Guides

| Guide | Service | Description |
|-------|---------|-------------|
| [Feishu Setup](FEISHU_SETUP.md) | [Feishu / Lark](https://open.feishu.cn/) | Feishu bot channel — receive and send messages via Feishu |

## Overview

esp32claw supports multiple IM channels for interacting with the AI agent. Each guide below walks through obtaining API credentials, configuring esp32claw (build-time or runtime), and verifying the integration.

All credentials can be set in two ways:

1. **Build-time** — define in `main/mimi_secrets.h` and rebuild
2. **Runtime** — use serial CLI commands (saved to NVS flash, no rebuild needed)

See [mimi_secrets.h.example](../../main/mimi_secrets.h.example) for the full list of configurable secrets.

