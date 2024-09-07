import matplotlib.pyplot as plt

def leer_puntos_desde_txt(archivo):
    puntos = []
    with open(archivo, 'r') as file:
        for linea in file:
            # Eliminar espacios en blanco y saltos de línea
            linea = linea.strip()
            if linea:  # Asegurarse de que la línea no esté vacía
                # Convertir la línea en un par de flotantes
                punto = tuple(map(float, linea.strip('()').split(',')))
                puntos.append(punto)
    return puntos

def graficar_puntos_con_indices(puntos, ax, titulo, trazar_lineas=False):
    x_values, y_values = zip(*puntos)
    ax.scatter(x_values, y_values, color='blue')

    # Agregar los índices de los puntos
    for i, (x, y) in enumerate(puntos):
        ax.text(x, y, f'{i}', fontsize=12, ha='right', va='bottom', color='red')

    # Trazar líneas entre los puntos si se especifica
    if trazar_lineas:
        for i in range(len(puntos) - 1):
            x_values_line = [puntos[i][0], puntos[i+1][0]]
            y_values_line = [puntos[i][1], puntos[i+1][1]]
            ax.plot(x_values_line, y_values_line, color='green')

    ax.set_title(titulo)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.grid(True)

# Especifica los nombres de los archivos .txt
archivo_txt_1 = 'bodies_before_sorting0.txt'
archivo_txt_2 = 'bodies_after_sorting0.txt'

# Leer los puntos desde los archivos
puntos1 = leer_puntos_desde_txt(archivo_txt_1)
puntos2 = leer_puntos_desde_txt(archivo_txt_2)

# Crear los subplots
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 8))

# Graficar los puntos con sus índices en cada subplot
graficar_puntos_con_indices(puntos1, ax1, 'Before' , trazar_lineas=True)
graficar_puntos_con_indices(puntos2, ax2, f'{archivo_txt_2}', trazar_lineas=True)

# Mostrar la gráfica
plt.tight_layout()
plt.show()
