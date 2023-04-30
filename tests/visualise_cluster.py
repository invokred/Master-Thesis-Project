import folium
import random
# from urllib.request import urlopen, Request
from branca.element import Figure

used_colors = set([])
def randomColor():
    global used_colors
    i = 0
    color = "#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)])
    # print(color, len(used_colors))
    while (color in used_colors) and (i<10):
        i = i+1
        color = "#"+''.join([random.choice('0123456789ABCDEF') for j in range(6)])

    if(i>=10):
        raise "Ran out of tries for colors"
    
    used_colors.add(color)
    return color
    
if __name__=="__main__":
    random.seed(10)

    pts = open("input.txt", "r")

    numclusters = int(pts.readline())

    color = randomColor()
    fig5=Figure(height=550,width=750)
    m5=folium.Map(location=[12.97095, 77.59058],zoom_start=100)
    fig5.add_child(m5)
    # map = folium.Map(location=[0, -88.0815])
    # folium.Marker(location=[42.1649, -88.0815], popup =  'Depot').add_to(map)
    listofclusters = []
    print(numclusters)
    while numclusters:
        numclusters-=1
        cluster_index = int(pts.readline())
        num_packages = int(pts.readline())
        listofpoints = []
        while num_packages:
            num_packages -= 1
            loc = pts.readline()
            depot_x_loc = float(loc.split(" ")[0])
            depot_y_loc = float(loc.split(" ")[1])
            listofpoints.append([depot_x_loc, depot_y_loc])
            listofclusters.append(listofpoints)
        # line = pts.readline()
   
    # print(listofclusters)
    
    k = len(listofclusters)
    # print(type(k))

    while k:
        k-=1
        f=folium.FeatureGroup("Vehicle {}".format(len(listofclusters)-k))
        popup="Path of Vehicle {}".format( len(listofclusters)-k-1)
        tooltip='Vehicle {}'.format(len(listofclusters)-k-1)
        col=randomColor()
        weight=10 
        l=folium.vector_layers.PolyLine(listofclusters[len(listofclusters)-k-1], popup = popup, tooltip = tooltip, color = col, weight=10 ).add_to(f)
        routes = listofclusters[len(listofclusters)-k-1]
        sizeroute = len(routes)
        while sizeroute:
            sizeroute-=1
            m = folium.Marker(location=routes[len(routes)-1-sizeroute],popup='Point {}'.format(len(routes)-1-sizeroute), icon=folium.Icon(color=col,icon='none'))
            m.add_to(m5)
        l.add_to(m5)



    m5.save('clusters.html')

    exit()
    
    # # depot location
    # loc = pts.readline()
    # depot_x_loc = float(loc.split(" ")[0])
    # depot_y_loc = float(loc.split(" ")[1])

    # clusters = [int(v) for v in open('output (2).txt').read().split()]


    # pts.readline() # the dimensions of the bin
    # num_ct = int(pts.readline()) # the number of packages
    # colors = []
    # for i in range(num_ct):
    #     colors.append(randomColor())

    # results = []
    # map = folium.Map(location=[42.150, -88.034])
    # folium.Marker(location=[42.129, -88.027], popup =  'Depot').add_to(map)


    # for i in range(num_ct):
    #     line = pts.readline()
    #     pt_x_loc = float(line.split(" ")[0])
    #     pt_y_loc = float(line.split(" ")[1])
        
    #     # results.append([pt_x_loc, pt_y_loc, colors[clusters[i]]])
    #     folium.Circle(
    #         radius = 50, 
    #         location=[pt_x_loc, pt_y_loc],
    #         fill = False,
    #         # popup = f"lat={pt_x_loc}, lng={pt_y_loc}, cluster={clusters[i]}",
    #         color = colors[clusters[i]]
    #     ).add_to(map)

    # map.save('clusters.html')



