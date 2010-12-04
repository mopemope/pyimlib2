import pyimlib2

img1 = pyimlib2.open("/home/ma2/Dropbox/DSC00601.JPG")
img2 = img1.scaled_image(120, 120)
img2.save('/tmp/1.jpg', 'jpeg')


"""
img1 = pyimlib2.create_image(400,400)
img1.draw_rectangle(0, 0, 400, 400, color=(255, 0, 0, 255))

img2 = pyimlib2.create_image(200,200)
img2.draw_ellipse(0, 0, 200, 200, color=(0, 255, 0, 255), fill=True)

#img2.blend_image(img1)
img1.blend_image(img2)
#img2.blend_image(img1, src=(0, 0, 100, 100), dest=(0, 0, 100, 100))
#img1.blend_image(img2, src=(0, 0, 100, 100), dest=(0, 0, 100, 100))

img1.save('/tmp/1.jpg', 'jpeg')
img2.save('/tmp/2.jpg', 'jpeg')

print img1
"""
