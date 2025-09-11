#!/bin/bash

# Create icons directory
mkdir -p src/gfx/icons

# Function to create a simple icon
create_icon() {
    local text="$1"
    local filename="$2"
    local size="${3:-32x32}"
    
    # Create a simple colored square with text
    convert -size $size xc:lightgray \
        -gravity center \
        -pointsize 12 \
        -fill black \
        -draw "text 0,0 '$text'" \
        "src/gfx/icons/$filename.png"
    
    echo "Created $filename.png"
}

# Create all the missing icons
create_icon "M" "music"
create_icon "MOV" "movies"
create_icon "TV" "tv"
create_icon "POD" "podcasts"
create_icon "AUD" "audiobooks"
create_icon "REC" "recent"
create_icon "RP" "recent_played"
create_icon "TOP" "top25"
create_icon "▶" "play"
create_icon "⏸" "pause"
create_icon "⏹" "stop"
create_icon "⏮" "previous"
create_icon "⏭" "next"
create_icon "🔊" "volume"
create_icon "🔍" "search"
create_icon "📋" "list_view"
create_icon "📁" "album_view"
create_icon "🎵" "cover_flow"
create_icon "SYN" "syndromatic_logo"
create_icon "📻" "radio"
create_icon "🏠" "home_sharing"
create_icon "🧠" "genius"
create_icon "🎧" "dj"
create_icon "90s" "90s"
create_icon "🎼" "classical"
create_icon "🎬" "videos"
create_icon "⭐" "top_rated"
create_icon "📁" "folder"
create_icon "📝" "playlist"

echo "All icons created successfully!" 