import tkinter as tk
from tkinter import filedialog, messagebox
from PIL import Image, ImageDraw, ImageTk
import os

class ImageAnnotator:
    def __init__(self, root):
        self.root = root
        self.root.title("Image Annotator")
        
        # Переменные
        self.image = None
        self.image_path = None
        self.canvas = None
        self.points = []
        self.rectangles = []
        self.drawn_shapes = []  # Для хранения ID нарисованных объектов на Canvas
        self.save_index = 1     # Индекс для сохранения файлов

        # Интерфейс
        self.load_button = tk.Button(root, text="Загрузить изображение", command=self.load_image)
        self.load_button.pack()

        self.save_button = tk.Button(root, text="Сохранить результат", command=self.save_image, state=tk.DISABLED)
        self.save_button.pack()

        # Связываем клавишу Backspace с функцией отмены
        self.root.bind("<BackSpace>", self.undo)

    def load_image(self):
        file_path = filedialog.askopenfilename(filetypes=[("Image files", "*.png *.jpg *.jpeg *.bmp *.tiff")])
        if not file_path:
            return
        
        self.image_path = file_path
        self.image = Image.open(self.image_path)
        self.display_image()
        
        # Сброс переменных
        self.points = []
        self.rectangles = []
        self.drawn_shapes = []
        self.save_button.config(state=tk.NORMAL)

    def display_image(self):
        if self.canvas:
            self.canvas.destroy()

        tk_image = ImageTk.PhotoImage(self.image)
        self.canvas = tk.Canvas(self.root, width=tk_image.width(), height=tk_image.height())
        self.canvas.pack()
        self.canvas.create_image(0, 0, anchor=tk.NW, image=tk_image)
        self.canvas.image = tk_image
        self.canvas.bind("<Button-1>", self.on_click)

    def on_click(self, event):
        # Сохраняем точку и рисуем её на Canvas
        self.points.append((event.x, event.y))
        point_id = self.canvas.create_oval(event.x - 2, event.y - 2, event.x + 2, event.y + 2, fill="blue")
        self.drawn_shapes.append(point_id)

        # Если выбрано 4 точки, рисуем четырехугольник
        if len(self.points) == 4:
            self.draw_rectangle()
            self.points = []

    def draw_rectangle(self):
        # Рисуем полигон на Canvas
        for i in range(4):
            line_id = self.canvas.create_line(self.points[i][0], self.points[i][1],
                                              self.points[(i+1)%4][0], self.points[(i+1)%4][1], 
                                              fill="red", width=2)
            self.drawn_shapes.append(line_id)

        # Сохраняем прямоугольник для бинаризованного изображения
        self.rectangles.append(self.points)

    def undo(self, event=None):
        if self.points:
            # Удаляем последнюю точку
            self.points.pop()
            if self.drawn_shapes:
                self.canvas.delete(self.drawn_shapes.pop())
        elif self.rectangles:
            # Удаляем последний четырехугольник
            self.rectangles.pop()
            for _ in range(4):  # Удаляем линии четырехугольника
                if self.drawn_shapes:
                    self.canvas.delete(self.drawn_shapes.pop())

    def save_image(self):
        # Создаем бинаризованное изображение
        binary_image = Image.new("1", self.image.size, 0)
        draw = ImageDraw.Draw(binary_image)
        for rect in self.rectangles:
            draw.polygon(rect, fill=1)

        # Генерируем уникальное имя файла
        save_dir = os.path.dirname(os.path.abspath(__file__))
        while True:
            save_path = os.path.join(save_dir, f"result_{self.save_index}.png")
            if not os.path.exists(save_path):  # Проверяем, не существует ли файл
                break
            self.save_index += 1

        # Сохраняем файл
        binary_image.save(save_path)
        messagebox.showinfo("Успех", f"Изображение сохранено: {save_path}")

if __name__ == "__main__":
    root = tk.Tk()
    app = ImageAnnotator(root)
    root.mainloop()

