% This is where you name your data file
T = readtable('Outline.csv');
lat   = T.Latitude;
lon   = T.Longitude;
depth = T.Distance_m_;

valid = isfinite(lat) & isfinite(lon) & isfinite(depth);
lat   = lat(valid);
lon   = lon(valid);
depth = depth(valid);

%Conversion from Latitude and Longitude to distances in meters
R = 6371000;
lat0 = mean(lat);
lon0 = mean(lon);

lat_rad  = deg2rad(lat);
lon_rad  = deg2rad(lon);
lat0_rad = deg2rad(lat0);
lon0_rad = deg2rad(lon0);

x = R * (lon_rad - lon0_rad) .* cos(lat0_rad);
y = R * (lat_rad - lat0_rad);

%inverts depth
z = -depth;

n = 200;
xg = linspace(min(x), max(x), n);
yg = linspace(min(y), max(y), n);
[X, Y] = meshgrid(xg, yg);

F = scatteredInterpolant(x, y, z, 'natural', 'none');
Z = F(X, Y);

figure;
surf(X, Y, Z, 'EdgeColor', 'none');

%Define your labels
xlabel('Distance East (m)');
ylabel('Distance North (m)');
zlabel('Depth (m)');
title('Timber Linn Lake');
colorbar;
%Change Z ascpect for better visual of depth
axis tight;
axis equal;
daspect([1 1 .5]); 

view(3);
grid on;
camlight headlight;
lighting gouraud;
set(gca, 'ZDir', 'normal');   
hold on;
surf(X, Y, zeros(size(Z)), ...
    'FaceAlpha', 0.15, 'EdgeColor', 'none');
hold off;

