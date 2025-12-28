# AMD GPU Firmware Blobs

This directory contains firmware files for the AMD Radeon RX 6600 (Navi 23 / "dimgrey cavefish").

## Required Files

- `dimgrey_cavefish_ce.bin` - Command Engine firmware
- `dimgrey_cavefish_me.bin` - MicroEngine firmware
- `dimgrey_cavefish_mec.bin` - MEC (Compute) firmware
- `dimgrey_cavefish_pfp.bin` - Pre-Fetch Parser firmware
- `dimgrey_cavefish_rlc.bin` - Run List Controller firmware
- `dimgrey_cavefish_sdma.bin` - SDMA engine firmware
- `dimgrey_cavefish_vcn.bin` - Video Core Next firmware

## Source

These files are available from the [linux-firmware repository](https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git).

## Obtaining Firmware

```bash
# From repository root
git clone --depth 1 https://git.kernel.org/pub/scm/linux/kernel/git/firmware/linux-firmware.git /tmp/linux-firmware
cp /tmp/linux-firmware/amdgpu/dimgrey_cavefish_*.bin deps/firmware/
```

## License

These firmware files are redistributable under the AMD GPU firmware license included with each file.

## Version Tracking

Create a `VERSION` file documenting:
- Linux firmware repository commit hash
- Date obtained
- File checksums (SHA256)

## Status

⚠️ **Not yet populated** - Firmware files will be added during Phase 4 (Hardware Support)
