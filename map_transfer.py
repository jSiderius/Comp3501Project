from PIL import Image

def read_png(file_path):
    # Open the PNG image
    with Image.open(file_path) as img:
        # Get the width and height of the image
        width, height = img.size

        arr = []
        # Iterate through each pixel
        for y in range(height):
            arr.append([])
            for x in range(width):
                # Get the RGB values of the pixel
<<<<<<< HEAD
                pixel_intensity = img.getpixel((x, y))[0]

                # temp_height = ((pixel_color[0] + pixel_color[1] + pixel_color[2])/765)*5
                temp_height = (255 - pixel_intensity) / 255.0 * 150; 
=======
                pixel_color = img.getpixel((x, y))

                temp_height = ((pixel_color[0] + pixel_color[1] + pixel_color[2])/765)*5
>>>>>>> 497e5860fc00dce238d1a3e9764e57100c9f11c3
                arr[y].append(temp_height)
                
                # Print the color of the pixel
                # print(f"Pixel at ({x}, {y}): RGB = {pixel_color}")

        save_to_text_file(arr, "height_map.txt")
        # print(arr)

def save_to_text_file(matrix, file_path):
    with open(file_path, 'w') as file:
        for row in matrix:
            # Convert each element in the row to a string and join them with spaces
            row_str = ' '.join(map(str, row))
            # Write the row string to the file
            file.write(row_str + '\n')


if __name__ == "__main__":
<<<<<<< HEAD
    # image_path = "map3.jpg"
    image_path = "map4.png"
=======
    #image_path = "moon.jpg"
    image_path = "map2.png"
>>>>>>> 497e5860fc00dce238d1a3e9764e57100c9f11c3
    
    read_png(image_path)
