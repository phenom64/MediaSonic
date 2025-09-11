#!/usr/bin/env python3
"""
Simple script to create basic placeholder icons for MediaSonic
"""

from PIL import Image, ImageDraw, ImageFont
import os

def create_icon(text, filename, size=(32, 32), bg_color=(240, 240, 240), text_color=(80, 80, 80)):
    """Create a simple icon with text"""
    img = Image.new('RGBA', size, bg_color)
    draw = ImageDraw.Draw(img)
    
    # Try to use a system font, fallback to default
    try:
        font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 12)
    except:
        font = ImageFont.load_default()
    
    # Calculate text position to center it
    bbox = draw.textbbox((0, 0), text, font=font)
    text_width = bbox[2] - bbox[0]
    text_height = bbox[3] - bbox[1]
    x = (size[0] - text_width) // 2
    y = (size[1] - text_height) // 2
    
    # Draw text
    draw.text((x, y), text, fill=text_color, font=font)
    
    # Save the icon
    img.save(f"src/gfx/icons/{filename}.png")

def main():
    """Create all the missing icons"""
    icons = [
        ("M", "music"),
        ("MOV", "movies"),
        ("TV", "tv"),
        ("POD", "podcasts"),
        ("AUD", "audiobooks"),
        ("REC", "recent"),
        ("RP", "recent_played"),
        ("TOP", "top25"),
        ("▶", "play"),
        ("⏸", "pause"),
        ("⏹", "stop"),
        ("⏮", "previous"),
        ("⏭", "next"),
        ("🔊", "volume"),
        ("🔍", "search"),
        ("📋", "list_view"),
        ("📁", "album_view"),
        ("🎵", "cover_flow"),
        ("SYN", "syndromatic_logo"),
        ("📻", "radio"),
        ("🏠", "home_sharing"),
        ("🧠", "genius"),
        ("🎧", "dj"),
        ("90s", "90s"),
        ("🎼", "classical"),
        ("🎬", "videos"),
        ("⭐", "top_rated"),
        ("📁", "folder"),
        ("📝", "playlist"),
    ]
    
    # Create icons directory if it doesn't exist
    os.makedirs("src/gfx/icons", exist_ok=True)
    
    for text, filename in icons:
        create_icon(text, filename)
        print(f"Created {filename}.png")

if __name__ == "__main__":
    main() 