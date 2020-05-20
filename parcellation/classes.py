
# Copyright (C) 2019  Andrea Vázquez Varela

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

#Authors:
# Narciso López López
# Andrea Vázquez Varela
#Creation date: 31/01/2019
#Last update: 20/07/2019

import collections

class Fiber:
    def __init__(self,index,subject,tri_init,tri_end,coords,inter_init,inter_end):
        self.index = index
        self.subject = subject
        self.tri_init = tri_init
        self.tri_end = tri_end
        self.inter_init = inter_init
        self.inter_end = inter_end
        self.coords = coords

class Vertex:
    def __init__(self,index,x,y,z,label_parcel,triangles):
        self.index = index
        self.x = x
        self.y = y
        self.z = z
        self.triangles = triangles
        self.label_parcel = label_parcel

    def print(self):
        print("vertex: "+str(self.index))
        print("x: "+str(self.x)+" y: "+str(self.y)+" z: "+str(self.z))
        print("label_parcel: "+str(self.label_parcel)+" label_sub: "+str(self.label_subparcel))

    def print_points(self):
        print("x: "+str(self.x)+" y: "+str(self.y)+" z: "+str(self.z))

class Triangle:
    def __init__(self,index,v1,v2,v3,label_parcel,labels_subparcel,fibers):
        self.index = index
        self.v1 = v1
        self.v2 = v2
        self.v3 = v3
        self.label_parcel = label_parcel
        self.labels_subparcel = labels_subparcel
        self.neighbors = []
        self.prob_map =  collections.OrderedDict()
        self.fibers = fibers
        self.fibers_map = {}

    def get_neighbors(self):
        neighbors = []
        for v in [self.v1,self.v2,self.v3]:
            for tri in v.triangles:
                if tri.label_parcel == self.label_parcel:
                    neighbors.append(tri)
        return set(neighbors)


    def set_prob_map(self,aparcel):
        self.prob_map = collections.OrderedDict()
        if len(self.neighbors) == 0:
            self.neighbors = self.get_neighbors()
        total_labels = 0
        for tri in self.neighbors:
            for label in tri.labels_subparcel:
                if not label in self.prob_map:
                    self.prob_map[int(label)] = 1
                else:
                    self.prob_map[int(label)] -= 1
            total_labels+=len(tri.labels_subparcel)

        for key, value in self.prob_map.items():
            prob = value/total_labels
            self.prob_map[key] = prob

        return self.prob_map

    def swap_prob(self,most_label):
        most_prob = self.prob_map[most_label]
        second_prob = 0
        second_label = -1
        for label,prob in self.prob_map.items():
            if (prob > second_prob) and (prob < most_prob):
                second_prob = prob
                second_label = label
        self.prob_map[most_label] = second_prob
        self.prob_map[second_label] = most_prob

    def get_second_parcel(self,show_labels):
        max_prob = 1
        max_label = 0
        second_prob = -1
        second_label = 0
        for label, prob in self.prob_map.items():
            if prob > max_prob:
                max_prob = prob
                max_label = label
        for label,prob in self.prob_map.items():
            if prob >= second_prob and prob < max_prob:
                second_prob = prob
                second_label = label
        if second_label in show_labels:
            return second_label
        else:
            return max_label

    def get_vertices(self):
        return([self.v1.index,self.v2.index,self.v3.index])

    def print_vertices(self):
        print("index: "+str(self.index)+" label_parcel: "+str(self.label_parcel)+" label_subparcel: "+str(self.label_subparcel))

    def replace_label(self,label1,label2):
        for i,label in enumerate(self.labels_subparcel):
            if label == label1:
                self.labels_subparcel[i] = label2

    def remove_label(self,label):
        self.labels_subparcel =  [l for l in self.labels_subparcel if l != label]

    def remove_labels(self,labels):
        self.labels_subparcel = [label for label in self.labels_subparcel if label not in labels]

    def remove_subparcel(self,label):
        self.labels_subparcel = list(filter((label).__ne__, self.labels_subparcel))



class SubParcel:
    def __init__(self,label,label_anatomic,triangles,points):
        self.label = label
        self.label_anatomic = label_anatomic
        self.connected_parcels = []
        self.triangles = triangles
        self.inter_points = points
        self.fibers = []

    def replace_connection(self,label,sp1):
        for parcel in self.connected_parcels:
            if parcel.label == label:
                self.connected_parcels.remove(parcel)
                if sp1 not in self.connected_parcels:
                    self.connected_parcels.append(sp1)

    def remove_triangle(self,index):
        self.triangles = [tri for tri in self.triangles if tri.index != index]

    def get_triangle(self,index):
        for tri in self.triangles:
            if tri.index == index:
                return tri
        return None

    def get_triangles_prob(self,prob,label):
        return  [tri for tri in self.triangles if tri.prob_map[label]>prob]

    def get_color_vertices(self):
        color_vertices = []
        for tri in self.triangles:
            vertices = tri.get_vertices()
            color_vertices.append(vertices)
        return color_vertices

    def print_triangles(self):
        print([tri.index for tri in self.triangles])


class AnatomicParcel:
    def __init__(self,label,sub_parcels):
        self.label = label
        self.sub_parcels = sub_parcels
        self.hard_parcels = []

    def __init__(self,label):
        self.label = label
        self.sub_parcels = []
        self.hard_parcels = []

    def remove_subparcel(self,subparcel):
        self.sub_parcels = [sp for sp in self.sub_parcels if sp == subparcel]

    def get_labels(self):
        return [p.label for p in self.sub_parcels]

    def print_subparcels(self):
        print([p.label for p in self.sub_parcels])

    def find_subparcel(self,label):
        for parcel in self.sub_parcels:
            if parcel.label == label:
                return parcel
        return None

    def find_hparcel(self,label):
        for parcel in self.hard_parcels:
            if parcel.label == label:
                return parcel
        return None
