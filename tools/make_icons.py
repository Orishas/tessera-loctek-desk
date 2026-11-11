#!/usr/bin/env python3
"""Turns the icons in tools/svg/ (Material Design Icons, Apache 2.0) into components/loctek_desk/icons.h: one A8
mask per icon, how much of each pixel the icon covers. The screen paints a mask in any theme colour, so the Sit and
Stand keys carry the icons of their Home Assistant card in the colours the other keys' icons have.

    pip install cairosvg pillow
    python3 tools/make_icons.py
"""
import io
from pathlib import Path

import cairosvg
from PIL import Image

SIZE = 64  # pixels a side; the tile scales an icon to its key
ICONS = ['seat', 'human-handsup']  # the files in tools/svg/

here = Path(__file__).resolve().parent
out = here.parent / 'components' / 'loctek_desk' / 'icons.h'
lines = [
    '#pragma once',
    '// Made by tools/make_icons.py from Material Design Icons (Apache 2.0). Do not edit by hand.',
    '#include <cstdint>',
    '',
    'namespace esphome::loctek_desk {',
    '',
    f'static constexpr int ICON_SIZE = {SIZE};',
    '',
]
for name in ICONS:
    svg = (here / 'svg' / f'{name}.svg').read_text().replace('<path ', '<path fill="#000000" ')
    png = cairosvg.svg2png(bytestring=svg.encode(), output_width=SIZE, output_height=SIZE)
    alpha = Image.open(io.BytesIO(png)).convert('RGBA').getchannel('A').tobytes()
    ident = 'ICON_' + name.upper().replace('-', '_')
    lines.append(f'// ICON_SIZE * ICON_SIZE bytes, row by row: {name}')
    lines.append(f'static const uint8_t {ident}[] = {{')
    for i in range(0, len(alpha), 24):
        lines.append('  ' + ', '.join(f'0x{b:02x}' for b in alpha[i:i + 24]) + ',')
    lines.append('};')
    lines.append('')
lines += ['}  // namespace esphome::loctek_desk', '']
out.write_text('\n'.join(lines))
print(f'{out}: {len(ICONS)} icons of {SIZE}x{SIZE}')
