from pygame import *

init()

screen = display.set_mode((800,600))
screen.fill((177,177,177))
ft = image.load('gen/gen41.png')
rect = (10,10,ft.get_width(),ft.get_height())
screen.blit(ft,rect)

r = True
while r:

	for e in event.get():
		if e.type == QUIT:
			r = False

	display.flip()

quit()
