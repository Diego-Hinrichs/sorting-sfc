import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


fb_data = [
        (-0.010928, 0.551982), (-0.308721, -0.039014), (-0.069660, 0.062500), 
        (-0.202704, 0.555177), (-0.078742, 0.332970), (0.105676, -0.159392), 
        (0.485504, -0.893199), (0.147332, 0.330641), (-0.315056, -0.076573), 
        (-0.835173, -0.198619), (0.209912, -0.383347), (0.207029, 0.463339), 
        (0.063129, 0.314005), (-1.000298, -0.585756), (0.535395, -0.957439), 
        (-0.228751, 0.089159), (0.315534, 0.639237), (0.182402, -0.750917), 
        (-1.000335, -0.636428), (-1.000131, -0.565764), 
]

bh_data = [
        (-0.288232, 0.511211), (-0.142048, 0.056511), (0.039850, -0.032681), 
        (-0.194803, 0.608204), (-0.014339, 0.155962), (0.106336, -0.155568), 
        (0.436006, -0.926922), (0.131620, 0.372816), (-0.246600, 0.087507), 
        (-0.843382, -0.106186), (0.215549, -0.374171), (0.128628, 0.355489), 
        (0.014775, 0.251381), (-1.035960, -0.395854), (0.544969, -1.017368), 
        (-0.228903, 0.053890), (0.376920, 0.544726), (0.229073, -0.810343), 
        (-1.473373, -0.768267), (-1.117253, -0.569508), 
]

n_1 = 20
step_1 = 4000
fb_data_1 = np.array(fb_data)
bh_data_1 = np.array(bh_data)
# fb_data_2 = np.array(fb_data_2)
# bh_data_2 = np.array(bh_data_2)
# fb_data_3 = np.array(fb_data_3)
# bh_data_3 = np.array(bh_data_3)

# Configuración de la cuadrícula 1x3
fig, axs = plt.subplots(1, 3, figsize=(18, 6))

# Datos para los tres gráficos
datasets = [
        (fb_data_1, bh_data_1, n_1, step_1),
        # (fb_data_2, bh_data_2, n_2, step_2),
        # (fb_data_3, bh_data_3, n_3, step_3),
]

# Itera y dibuja cada gráfico
for i, (fb_positions, bh_positions, n, step) in enumerate(datasets):
        fb_x, fb_y = fb_positions[:, 0], fb_positions[:, 1]
        bh_x, bh_y = bh_positions[:, 0], bh_positions[:, 1]

        axs[i].scatter(fb_x, fb_y, color='blue', label='FB', marker='o')
        axs[i].scatter(bh_x, bh_y, color='red', label='BH', marker='x')
        for j in range(len(fb_x)):
                axs[i].plot([fb_x[j], bh_x[j]], [fb_y[j], bh_y[j]], color='gray', linestyle='--', linewidth=0.8)

        # Etiquetas y leyenda para cada subplot
        axs[i].set_xlabel("Posición X")
        axs[i].set_ylabel("Posición Y")
        axs[i].set_title(f"Diferencia de posiciones entre FB y BH\n Num Points = {n} Steps = {step}")
        axs[i].legend()
        axs[i].grid(True)

plt.tight_layout()
plt.show()