import matplotlib.pyplot as plt
import numpy as np

# Cargar los datos desde un archivo .txt
steps = []
errors = []
with open('error.txt', 'r') as file:
    lines = file.readlines()
    for line in lines:
        step, error = line.strip().split(';')
        steps.append(int(step))
        errors.append(float(error))

# Crear la gráfica
plt.figure(figsize=(10, 6))
plt.plot(steps, errors, marker='o', linestyle='-', color='b', label='Error promedio')
plt.axhline(y=0.0001, color='r', linestyle='--', label='Tolerancia (0.0001)')

# Encontrar el primer paso donde el error cruza la tolerancia
for i in range(len(errors)):
    if errors[i] >= 0.0001:
        plt.axvline(x=steps[i], color='g', linestyle='--', label=f'Intersección en paso {steps[i]}')
        break

# Etiquetas y título
plt.xlabel('Paso')
plt.ylabel('Error promedio')
plt.title('Evolución del Error Promedio por Paso')
plt.yscale('log')  # Escala logarítmica para visualizar mejor los valores pequeños
plt.ylim(1e-10, 2)  # Establecer el rango del eje Y
plt.legend()
plt.grid(True, which="both", linestyle='--', linewidth=0.5)

# Mostrar la gráfica
plt.show()
