original_image=%1

for size in 16 32 64 128 256 512 1024; do
  magick icon_1024x1024.png -resize ${size}x${size} icon.iconset/icon_${size}x${size}.png
  cp icon.iconset/icon_${size}x${size}.png icon.iconset/icon_$((size/2))x$((size/2))@2x.png
done


