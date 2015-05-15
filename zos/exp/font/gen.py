from __future__ import print_function
from PIL import Image
import PIL.ImageOps as po
import PIL.ImageDraw as pd
import PIL.ImageFont as pf
import PIL.ImageChops as pc

'''
f = Image.open("Inconsolata2.png")
w,h = f.size
sx = 0
sy = 0
dx = 72
dy = 58

nx = 10
ny = 29

fx = 28
fy = 16
fw = 16
fh = 32

imgw = 32
imgh = 12
img = Image.new('RGBA',(fw*imgw,fh*imgh))

fl = []

y = sy
for i in range(ny):
	x = sx
	for j in range(nx):
		fn = i*nx+j+33
		box = (x,y,x+dx,y+dy)		
		x += dx
		new = f.crop(box)
		new.save("gen/%sbig.png"%fn)
		box2 = (fx,fy,fx+fw,fy+fh)
		ft = new.crop(box2)
		fl.append((fn,ft))
		ft.save("gen/%s.png"%fn)
	y+=dy

import math
for n,ft in fl:
	img.paste(ft,(n%imgw*fw,n/imgw*fh))
	#print(n,(n%imgw*fw,n/imgh*fh))

# INV
r,g,b,a = img.split()
rgb = Image.merge('RGB',(r,g,b))
invrgb = PIL.ImageOps.invert(rgb)
r,g,b = invrgb.split()
inv = Image.merge('RGBA',(r,g,b,a))
black = Image.new('RGBA',(img.size),'black')

Image.blend(black,inv,0.01).save("genb.png")
#INV END
inv.save("geninv.png")
img.save("gen.png")

'''
import string

alltext = ""
for i in range(128):
	c = chr(i)
	if c not in string.printable:
		c = ""
	elif ord(c) < 20:
		c = ""
	alltext += c


path = '/usr/share/fonts/truetype/inconsolata/Inconsolata.otf'
font = pf.truetype(path,18)

def render(text):
	w,h = font.getsize(text)

	bg = Image.new('RGB',(w,h),(0,0,0))
	al = Image.new("L",bg.size,'black')

	#pd.Draw(bg).text((0,0),text,font=font,fill=(0,0,0))

	te = Image.new('L',bg.size,0)
	dr = pd.Draw(te)
	dr.text((0,0),text,font=font,fill="white")

	al = pc.lighter(al,te)

	cl = Image.new("RGBA",bg.size,(255,255,255))
	ma = Image.eval(te,lambda p: 255*(int(p!=0)))
	bg = Image.composite(cl,bg,ma)

	bg.putalpha(al)
	#print(repr(bg.tostring()))
	#bg.save("genf.png")
	return bg


def genc(char):
	#print(repr(s))
	im = render(char)
	im.save("gen/gen%d.png"%ord(char))
	data = list(im.getdata())
	cs = "uint32_t _font_%d[9*20] =\n{\n"%ord(char)
	for y in range(20):
		for x in range(9):
			i = y*9+x
			r,g,b,a = data[i]
			num = ''.join([ "%02X"%x for x in a,r,g,b ])
			cs+="0x%s,"%num
		cs+='\n'

	cs += '};\n'

	return cs

f = open('a.txt','w')

for i in alltext:
	f.write(genc(i))


glyph = "{"
for i in range(128):
	if i%8==0: glyph+='\n'
	if chr(i) not in alltext:
		glyph+="0, "
	else:
		glyph+='_font_%d, '%i
glyph+='\n};'

f.write(glyph)

f.close()