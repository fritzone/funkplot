from os.path import exists
import shutil
import glob, os

## Code to fetch all the nice palettes and copy to upals
# palettes = ["BLUES", "BlueSerpent", "BlueSoo", "Calm", "Candy", "ChromaDepth", "ChoppedGold", "chroma", "CLOUDS2", "COLDFIRE", "colorbrewer*", "colorschemer*", "colourlovers*", "Combined018", "design-seeds", "fade*", "FIRE", "firestrm", "FLAMEY", "fluid*", "froth6", "froth3", "giger*", "GREENOUT", "GREENIN", "GREENS", "grey", "IFS*", "iq*", "Jason14", "Jason8", "kuler*", "Morgan*", "Movie*", "orbit*", "Quat*", "RainbowSmooth12"]
# for p in palettes:
# 	print("Working on " + p)
# 	if p.find("*") != -1:
# 		allfiles = glob.glob("palettes/" + p)
# 		for f in allfiles:
# 			shutil.copy(f, "upals")
# 	else:
# 		fn = "palettes/" + p + ".map"
# 		print(fn)
# 		if exists(fn):
# 			shutil.copy(fn, "upals")
# 		else:
# 			fn = "palettes/" + p + ".MAP"
# 			if exists(fn):
# 				shutil.copy(fn, "upals")
# 			else:
# 				print("Cannot find:" + p)

allpalettes = glob.glob("upals/*")
for f in allpalettes:
	res = any(ele.isupper() for ele in f)
	if res:
		os.remove(f)