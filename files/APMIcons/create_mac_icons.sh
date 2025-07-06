for size in 16 32 64 128 256 512 1024; do
#  magick ap_icon_original_1024x1024.png -resize ${size}x${size} \( -size ${size}x${size} xc:none -draw "roundrectangle 0,0,$((size-1)),$((size-1)),$((size/6)),$((size/6))" \) -compose DstIn -composite icon_${size}x${size}_rounded.png
  magick ap_icon_original_1024x1024.png -resize ${size}x${size} \
    \( -size ${size}x${size} xc:none -draw "fill white roundrectangle 0,0,$((size-1)),$((size-1)),$((size/6)),$((size/6))" \) \
    -alpha off -compose CopyOpacity -composite icon.iconset/icon_${size}x${size}.png
  cp icon.iconset/icon_${size}x${size}.png icon.iconset/icon_$((size/2))x$((size/2))@2x.png
done


