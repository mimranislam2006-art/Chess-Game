"""
generate_pieces.py
──────────────────
Generates all 12 chess piece PNG images (128×128 px) using only the
Python standard library + tkinter (built-in on most systems).

Run:  python generate_pieces.py
Output: white_pawn.png, black_queen.png, etc. in the same directory.

Requires: Pillow  (pip install Pillow)
"""

try:
    from PIL import Image, ImageDraw, ImageFont
except ImportError:
    print("Pillow not found. Installing...")
    import subprocess, sys
    subprocess.check_call([sys.executable, "-m", "pip", "install", "Pillow"])
    from PIL import Image, ImageDraw, ImageFont

import os

SIZE = 128

# Unicode chess symbols
SYMBOLS = {
    ('white', 'king'):   '\u2654',
    ('white', 'queen'):  '\u2655',
    ('white', 'rook'):   '\u2656',
    ('white', 'bishop'): '\u2657',
    ('white', 'knight'): '\u2658',
    ('white', 'pawn'):   '\u2659',
    ('black', 'king'):   '\u265A',
    ('black', 'queen'):  '\u265B',
    ('black', 'rook'):   '\u265C',
    ('black', 'bishop'): '\u265D',
    ('black', 'knight'): '\u265E',
    ('black', 'pawn'):   '\u265F',
}

BG_COLORS = {
    'white': (255, 255, 255, 0),   # transparent background
    'black': (255, 255, 255, 0),
}

PIECE_COLORS = {
    'white': (240, 240, 240),
    'black': (30,  30,  30),
}

OUTLINE_COLORS = {
    'white': (80,  80,  80),
    'black': (200, 200, 200),
}

def find_font(size):
    """Try to find a font that supports chess Unicode symbols."""
    candidates = [
        # Windows
        "C:/Windows/Fonts/seguisym.ttf",
        "C:/Windows/Fonts/arial.ttf",
        # Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
        # macOS
        "/System/Library/Fonts/Apple Symbols.ttf",
        "/Library/Fonts/Arial Unicode.ttf",
    ]
    for path in candidates:
        if os.path.exists(path):
            try:
                return ImageFont.truetype(path, size)
            except Exception:
                continue
    return ImageFont.load_default()

def generate_piece(color: str, piece: str, out_dir: str):
    img = Image.new("RGBA", (SIZE, SIZE), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)

    # Draw circular background
    margin = 8
    fill_color = PIECE_COLORS[color] + (255,)
    outline_color = OUTLINE_COLORS[color] + (255,)
    draw.ellipse(
        [margin, margin, SIZE - margin, SIZE - margin],
        fill=fill_color,
        outline=outline_color,
        width=3
    )

    # Draw symbol
    symbol = SYMBOLS[(color, piece)]
    font = find_font(72)

    # Get text bounding box
    bbox = draw.textbbox((0, 0), symbol, font=font)
    tw = bbox[2] - bbox[0]
    th = bbox[3] - bbox[1]
    tx = (SIZE - tw) // 2 - bbox[0]
    ty = (SIZE - th) // 2 - bbox[1]

    # Draw outline (shadow)
    shadow_color = OUTLINE_COLORS[color] + (180,)
    for dx, dy in [(-2,0),(2,0),(0,-2),(0,2),(-1,-1),(1,1),(-1,1),(1,-1)]:
        draw.text((tx + dx, ty + dy), symbol, font=font, fill=shadow_color)

    # Draw main symbol
    sym_color = PIECE_COLORS[color] if color == 'black' else (20, 20, 20)
    draw.text((tx, ty), symbol, font=font, fill=sym_color + (255,))

    filename = f"{color}_{piece}.png"
    path = os.path.join(out_dir, filename)
    img.save(path)
    print(f"  Generated: {filename}")

if __name__ == "__main__":
    out_dir = os.path.dirname(os.path.abspath(__file__))
    print(f"Generating chess piece images in: {out_dir}")
    for (color, piece) in SYMBOLS.keys():
        generate_piece(color, piece, out_dir)
    print("\nDone! All 12 piece images generated.")
    print("You can now build and run ChessGame.")
