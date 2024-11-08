'''
DISPLAY SVG MORPH PROGRAM
'''
import tkinter as tk
from tkinter import *
import cairosvg
import os
from PIL import Image, ImageTk


'''
FUNCTIONS
'''
class SVGAnimator:
    def __init__(self, root, png_files, label, delay=10):
        self.root = root
        self.png_files = png_files
        self.label = label  # Update to use the provided label
        self.delay = delay
        self.current_image_index = 0

        #animation updates
        self.update_image()

    def update_image(self):
        img = Image.open(self.png_files[self.current_image_index])
        
        #image resize to keep similar to original dimensions
        img = img.resize((400, 400), Image.LANCZOS)
        img_tk = ImageTk.PhotoImage(img)  
        
        self.label.config(image=img_tk, text='', width=400, height=400)
        self.label.image = img_tk 

        #move index for image
        self.current_image_index = (self.current_image_index + 1) % len(self.png_files)

        #update
        self.root.after(self.delay, self.update_image)

def convert_svgs_to_pngs(svg_directory, output_directory):
    #list svg files
    svg_files = sorted([f for f in os.listdir(svg_directory) if f.endswith('.svg')])
    os.makedirs(output_directory, exist_ok=True)

    #store png files
    png_files = []

    #converting svgs to pngs
    for svg_file in svg_files:
        svg_path = os.path.join(svg_directory, svg_file)
        png_file = os.path.splitext(svg_file)[0] + '.png'
        png_path = os.path.join(output_directory, png_file)

        #svg to png cairosvg
        cairosvg.svg2png(url=svg_path, write_to=png_path)
        png_files.append(png_path)

    #make gif
    output = 'output.gif'
    gif_path = make_gif(png_files, output, duration = 500)
    print(f"The GIF has been created at: {gif_path}")
    
    return png_files
            
def display_svg(svg_file, label, text):
    png_file = svg_file.replace('.svg', '.png')
    cairosvg.svg2png(url=svg_file, write_to=png_file)
    img = PhotoImage(file=png_file)
    label.config(image=img, text=text, compound="top", width=400, height=400)
    label.image = img  # Keep a reference

def example1(): #example 1 display
    display_svg("./ex1/tc_1_start.svg", first, "Phase 1")
    display_svg("./ex1/tc_1_final.svg", second, "Phase 2")
    
    svg_folder_path = './ex1/tc1'
    png_folder_path = './ex1/tc1/tc1_png'
    
    png_files = convert_svgs_to_pngs(svg_folder_path, png_folder_path)
    
    animator = SVGAnimator(root, png_files, third, delay=40)   

def example2(): #example 2 display
    display_svg("./ex2/frame_0.svg", first, "Phase 1")
    display_svg("./ex2/frame_99.svg", second, "Phase 2")
    
    svg_folder_path = './ex2/'
    png_folder_path = './ex2/ex2_png'
    png_files = convert_svgs_to_pngs(svg_folder_path, png_folder_path)
    
    animator = SVGAnimator(root, png_files, third, delay=40)    

def example3(): #example 2 display
    display_svg("./ex3/frame_0.svg", first, "Phase 1")
    display_svg("./ex3/frame_99.svg", second, "Phase 2")
    
    svg_folder_path = './ex3/'
    png_folder_path = './ex3/ex3_png'
    png_files = convert_svgs_to_pngs(svg_folder_path, png_folder_path)
    
    animator = SVGAnimator(root, png_files, third, delay=40)    
    

def example4(): #example 2 display
    display_svg("./ex4/frame_000.svg", first, "Phase 1")
    display_svg("./ex4/frame_100.svg", second, "Phase 2")
    
    svg_folder_path = './ex4/'
    png_folder_path = './ex4/ex4_png'
    png_files = convert_svgs_to_pngs(svg_folder_path, png_folder_path)
    
    animator = SVGAnimator(root, png_files, third, delay=40)    

def make_gif(png_files, output, duration = 500):
    images = [Image.open(img) for img in png_files]
    images[0].save(output, save_all=True, append_images=images[1:], duration=duration, loop=0)
    return output
    
'''
WIDGETS
'''

root = tk.Tk()
frame = tk.Frame(root)
root.title("Image Morphing")

# Buttons
button1 = tk.Button(root, text="Example 1", command=example1)
button2 = tk.Button(root, text="Example 2", command=example2)
button3 = tk.Button(root, text="Example 3", command=example3)
button4 = tk.Button(root, text="Example 4", command=example4)

# Widget Organization
button1.pack()
button2.pack()
button3.pack()
button4.pack()
label1 = tk.Label(root)
label2 = tk.Label(root)

bottom_frame = tk.Frame(root)
bottom_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)
    
first = tk.Label(bottom_frame, text="First phase", width=50, height=20, bg='gray', anchor='center')
second = tk.Label(bottom_frame, text="Second phase", width=50, height=20, bg='gray', anchor='center')
third = tk.Label(bottom_frame, text="Animation", width=50, height=20, bg='gray', anchor='center')

first.grid(row=0, column=0, padx=10, pady=10, sticky="nsew")
second.grid(row=0, column=1, padx=10, pady=10, sticky="nsew")
third.grid(row=0, column=2, padx=10, pady=10, sticky="nsew")

bottom_frame.grid_columnconfigure(0, weight=0)
bottom_frame.grid_columnconfigure(1, weight=0)
bottom_frame.grid_columnconfigure(2, weight=0)


label1.pack(side=tk.LEFT)
label2.pack(side=tk.LEFT)


root.mainloop()