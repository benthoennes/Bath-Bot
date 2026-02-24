import numpy as np
import csv
from vispy import app
app.use_app('PyQt5')
from vispy import scene
from scipy.interpolate import griddata
from vispy.color import get_colormap

# -------------------------
# SERIAL SETUP (DISABLED)
# -------------------------
# SERIAL_PORT = 'COM4'
# BAUD = 9600
# ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)

# -------------------------
# SAMPLE DATA SETUP
# -------------------------
sample_file = open('Sampledata.csv', 'r')
sample_reader = csv.reader(sample_file)
next(sample_reader)  # Skip header

# -------------------------
# CSV LOGGING SETUP
# -------------------------
csv_file = open('Bathymetry.csv', 'w', newline='')
writer = csv.writer(csv_file)

writer.writerow([
    "timestamp",
    "lat",
    "lon",
    "depth"
])

# -------------------------
# VISPY SETUP
# -------------------------

canvas = scene.SceneCanvas(keys='interactive', show=True)
view = canvas.central_widget.add_view()
view.camera = 'turntable'

surface = scene.visuals.SurfacePlot(
    x=np.zeros((10, 10)),
    y=np.zeros((10, 10)),
    z=np.zeros((10, 10)),
    shading='smooth',
    parent=view.scene)
surface.cmap = get_colormap('viridis')

axis = scene.visuals.XYZAxis(parent=view.scene)
# -------------------------
# DATA STORAGE
# -------------------------
points = []
origin = None

# -------------------------
# UPDATE FUNCTION
# -------------------------
def update_map(event):
    global points, origin

    # ---- READ FROM SAMPLE CSV INSTEAD OF SERIAL ----
    for _ in range(10):  # Read 10 rows per timer tick (simulates streaming)
        try:
            row = next(sample_reader)

            lat = float(row[1])
            lon = float(row[2])
            depth = float(row[3]) / 1000.0  # cm -> m

            if origin is None:
                origin = (lat, lon)

            x = (lat - origin[0]) * 11100
            y = (lon - origin[1]) * 11100 * np.cos(np.radians(lat))

            points.append([x, y, depth])
           

        except StopIteration:
            print("Reached end of file")
            return
            break
        except Exception as e:
            print("ERROR parsing row:", e)
            return

    # ---- BUILD SURFACE ----
    if len(points) < 50:
        return

    pts = np.array(points)
    x = pts[:, 0]
    y = pts[:, 1]
    z = pts[:, 2]

    grid_x, grid_y = np.mgrid[
        x.min():x.max():100j,
        y.min():y.max():100j
    ]

    grid_z = griddata(
        (x, y),
        z,
        (grid_x, grid_y),
        method='linear'
    )

    surface.set_data(x=grid_x, y=grid_y, z=-grid_z)

    valid = np.isfinite(grid_z)
    if np.any(valid):
        zneg = -grid_z

        zmin = float(np.nanmin(zneg))
        zmax = float(np.nanmax(zneg))
        if abs(zmax - zmin) < 1e-12:
            zmax = zmin + 1e-6

    surface.clim = (zmin, zmax)

    # Normalize z to 0..1 for colormap lookup
    zn = (zneg - zmin) / (zmax - zmin)
    zn = np.clip(zn, 0, 1)
    zn[~valid] = 0  # doesn’t matter much; those vertices are NaN anyway

    # Build RGBA colors from colormap and push to surface
    rgba = surface.cmap.map(zn.ravel()).reshape(grid_z.shape + (4,))
    surface.set_data(x=grid_x, y=grid_y, z=zneg, colors=rgba)
# -------------------------
# TIMER
# -------------------------
timer = app.Timer(interval=.001, connect=update_map, start=True)

# -------------------------
# CLEAN EXIT
# -------------------------
def close_event(ev):
    print("Closing file...")
    csv_file.close()
    sample_file.close()
    # ser.close()  # SERIAL DISABLED

canvas.events.close.connect(close_event)

# -------------------------
# RUN APP
# -------------------------
if __name__ == '__main__':
    app.run()
