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
                pixel_color = img.getpixel((x, y))

                if(pixel_color == (217, 87, 99, 255)):
                  arr[y].append(1)
                elif(pixel_color == (172, 50, 50, 255)):
                  arr[y].append(2) 
                elif(pixel_color == (118, 66, 138, 255)):
                  arr[y].append(3) 
                elif(pixel_color == (89, 86, 82, 255)):
                  arr[y].append(4) 
                elif(pixel_color == (105, 106, 106, 255)):
                  arr[y].append(5) 
                elif(pixel_color == (132, 126, 135, 255)):
                  arr[y].append(6) 
                elif(pixel_color == (203, 219, 252, 255)):
                  arr[y].append(7)
                elif(pixel_color == (0,0,0, 255)):
                  arr[y].append(8)
                elif(pixel_color == (48, 96, 130, 255)):
                  arr[y].append(9) 
                elif(pixel_color == (106, 190, 48, 255)):
                  arr[y].append(10) 
                elif(pixel_color == (55, 148, 110, 255)):
                  arr[y].append(11) 
                elif(pixel_color == (75,105, 47, 255)):
                  arr[y].append(12) 
                elif(pixel_color == (82, 75, 36, 255)):
                  arr[y].append(13) 
                elif(pixel_color == (50, 60, 37, 255)):
                  arr[y].append(14) 
                else: 
                  arr[y].append(0)

                
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
    image_path = "map2.png"
    
    read_png(image_path)
