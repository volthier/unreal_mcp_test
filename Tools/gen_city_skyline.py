"""Draw a Mega Man X-style dusk city skyline texture (sky + buildings + lit windows)."""
from PIL import Image, ImageDraw
import random
random.seed(7)
W,H=2048,1024
img=Image.new("RGB",(W,H))
d=ImageDraw.Draw(img)
# dusk sky vertical gradient (dark blue-purple top -> purple horizon)
for y in range(H):
    t=y/H
    r=int(18+40*t); g=int(15+22*t); b=int(45+40*t)
    d.line([(0,y),(W,y)],fill=(r,g,b))
# warm horizon glow
for y in range(int(H*0.70),H):
    t=(y-H*0.70)/(H*0.30)
    r=int(60+70*(1-abs(t-0.5)*2)); g=int(40+40*(1-abs(t-0.5)*2)); b=int(70+30*(1-abs(t-0.5)*2))
    d.line([(0,y),(W,y)],fill=(r,g,b))
# city skyline: building silhouettes
x=0
while x<W:
    bw=random.randint(70,150); bh=random.randint(int(H*0.35),int(H*0.85))
    bcol=(14,16,32)
    d.rectangle([x,H-bh,x+bw,H],fill=bcol)
    # windows grid (some lit orange/yellow)
    for wx in range(x+8, x+bw-6, 14):
        for wy in range(H-bh+12, H-12, 20):
            if random.random()<0.45:
                wcol=(255,190,70) if random.random()<0.7 else (255,120,60)
                d.rectangle([wx,wy,wx+7,wy+10],fill=wcol)
    # subtle roof edge
    d.line([(x,H-bh),(x+bw,H-bh)],fill=(30,34,55))
    x+=bw+random.randint(4,18)
img.save("Art/generated/city_skyline.png")
print("saved Art/generated/city_skyline.png", img.size)
