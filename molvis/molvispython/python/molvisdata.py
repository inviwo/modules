import inviwopy as ivw

default = {'rgb': [0, 0, 0], 'radius': 3.00}

atomColors = {
    "Xx": default,
    "H": {'rgb': [255, 255, 255], 'radius': 1.10},
    "He": {'rgb': [217, 255, 255], 'radius': 1.40},
    "Li": {'rgb': [204, 128, 255], 'radius': 1.81},
    "Be": {'rgb': [194, 255, 0], 'radius': 1.53},
    "B": {'rgb': [255, 181, 181], 'radius': 1.92},
    "C": {'rgb': [144, 144, 144], 'radius': 1.70},
    "N": {'rgb': [48, 80, 248], 'radius': 1.55},
    "O": {'rgb': [255, 13, 13], 'radius': 1.52},
    "F": {'rgb': [144, 224, 80], 'radius': 1.47},
    "Ne": {'rgb': [179, 227, 245], 'radius': 1.54},
    "Na": {'rgb': [171, 92, 242], 'radius': 2.27},
    "Mg": {'rgb': [138, 255, 0], 'radius': 1.73},
    "Al": {'rgb': [191, 166, 166], 'radius': 1.84},
    "Si": {'rgb': [240, 200, 160], 'radius': 2.10},
    "P": {'rgb': [255, 128, 0], 'radius': 1.80},
    "S": {'rgb': [255, 255, 48], 'radius': 1.80},
    "Cl": {'rgb': [31, 240, 31], 'radius': 1.75},
    "Ar": {'rgb': [128, 209, 227], 'radius': 1.88},
    "K": {'rgb': [143, 64, 212], 'radius': 2.75},
    "Ca": {'rgb': [61, 255, 0], 'radius': 2.31},
    "Sc": {'rgb': [230, 230, 230], 'radius': 2.30},
    "Ti": {'rgb': [191, 194, 199], 'radius': 2.15},
    "V": {'rgb': [166, 166, 171], 'radius': 2.05},
    "Cr": {'rgb': [138, 153, 199], 'radius': 2.05},
    "Mn": {'rgb': [156, 122, 199], 'radius': 2.05},
    "Fe": {'rgb': [224, 102, 51], 'radius': 2.05},
    "Co": {'rgb': [240, 144, 160], 'radius': 2.00},
    "Ni": {'rgb': [80, 208, 80], 'radius': 2.00},
    "Cu": {'rgb': [200, 128, 51], 'radius': 2.00},
    "Zn": {'rgb': [125, 128, 176], 'radius': 2.10},
    "Ga": {'rgb': [194, 143, 143], 'radius': 1.87},
    "Ge": {'rgb': [102, 143, 143], 'radius': 2.11},
    "As": {'rgb': [189, 128, 227], 'radius': 1.85},
    "Se": {'rgb': [255, 161, 0], 'radius': 1.90},
    "Br": {'rgb': [166, 41, 41], 'radius': 1.83},
    "Kr": {'rgb': [92, 184, 209], 'radius': 2.02},
    "Rb": {'rgb': [112, 46, 176], 'radius': 3.03},
    "Sr": {'rgb': [0, 255, 0], 'radius': 2.49},
    "Y": {'rgb': [148, 255, 255], 'radius': 2.40},
    "Zr": {'rgb': [148, 224, 224], 'radius': 2.30},
    "Nb": {'rgb': [115, 194, 201], 'radius': 2.15},
    "Mo": {'rgb': [84, 181, 181], 'radius': 2.10},
    "Tc": {'rgb': [59, 158, 158], 'radius': 2.05},
    "Ru": {'rgb': [36, 143, 143], 'radius': 2.05},
    "Rh": {'rgb': [10, 125, 140], 'radius': 2.00},
    "Pd": {'rgb': [0, 105, 133], 'radius': 2.05},
    "Ag": {'rgb': [192, 192, 192], 'radius': 2.10},
    "Cd": {'rgb': [255, 217, 143], 'radius': 2.20},
    "In": {'rgb': [166, 117, 115], 'radius': 2.20},
    "Sn": {'rgb': [102, 128, 128], 'radius': 1.93},
    "Sb": {'rgb': [158, 99, 181], 'radius': 2.17},
    "Te": {'rgb': [212, 122, 0], 'radius': 2.06},
    "I": {'rgb': [148, 0, 148], 'radius': 1.98},
    "Xe": {'rgb': [66, 158, 176], 'radius': 2.16},
    "Cs": {'rgb': [87, 23, 143], 'radius': 3.43},
    "Ba": {'rgb': [0, 201, 0], 'radius': 2.68},
    "La": {'rgb': [112, 212, 255], 'radius': 2.50},
    "Ce": {'rgb': [255, 255, 199], 'radius': 2.48},
    "Pr": {'rgb': [217, 255, 199], 'radius': 2.47},
    "Nd": {'rgb': [199, 255, 199], 'radius': 2.45},
    "Pm": {'rgb': [163, 255, 199], 'radius': 2.43},
    "Sm": {'rgb': [143, 255, 199], 'radius': 2.42},
    "Eu": {'rgb': [97, 255, 199], 'radius': 2.40},
    "Gd": {'rgb': [69, 255, 199], 'radius': 2.38},
    "Tb": {'rgb': [48, 255, 199], 'radius': 2.37},
    "Dy": {'rgb': [31, 255, 199], 'radius': 2.35},
    "Ho": {'rgb': [0, 255, 156], 'radius': 2.33},
    "Er": {'rgb': [0, 230, 117], 'radius': 2.32},
    "Tm": {'rgb': [0, 212, 82], 'radius': 2.30},
    "Yb": {'rgb': [0, 191, 56], 'radius': 2.28},
    "Lu": {'rgb': [0, 171, 36], 'radius': 2.27},
    "Hf": {'rgb': [77, 194, 255], 'radius': 2.25},
    "Ta": {'rgb': [77, 166, 255], 'radius': 2.20},
    "W": {'rgb': [33, 148, 214], 'radius': 2.10},
    "Re": {'rgb': [38, 125, 171], 'radius': 2.05},
    "Os": {'rgb': [38, 102, 150], 'radius': 2.00},
    "Ir": {'rgb': [23, 84, 135], 'radius': 2.00},
    "Pt": {'rgb': [208, 208, 224], 'radius': 2.05},
    "Au": {'rgb': [255, 209, 35], 'radius': 2.10},
    "Hg": {'rgb': [184, 184, 208], 'radius': 2.05},
    "Tl": {'rgb': [166, 84, 77], 'radius': 1.96},
    "Pb": {'rgb': [87, 89, 97], 'radius': 2.02},
    "Bi": {'rgb': [158, 79, 181], 'radius': 2.07},
    "Po": {'rgb': [171, 92, 0], 'radius': 1.97},
    "At": {'rgb': [117, 79, 69], 'radius': 2.02},
    "Rn": {'rgb': [66, 130, 150], 'radius': 2.20},
    "Fr": {'rgb': [66, 0, 102], 'radius': 3.48},
    "Ra": {'rgb': [0, 125, 0], 'radius': 2.83},
    "Ac": {'rgb': [112, 171, 250], 'radius': 2.00},
    "Th": {'rgb': [0, 186, 255], 'radius': 2.40},
    "Pa": {'rgb': [0, 161, 255], 'radius': 2.00},
    "U": {'rgb': [0, 143, 255], 'radius': 2.30},
    "Np": {'rgb': [0, 128, 255], 'radius': 2.00},
    "Pu": {'rgb': [0, 107, 255], 'radius': 2.00},
    "Am": {'rgb': [84, 92, 242], 'radius': 2.00},
    "Cm": {'rgb': [120, 92, 227], 'radius': 2.00},
    "Bk": {'rgb': [138, 79, 227], 'radius': 2.00},
    "Cf": {'rgb': [161, 54, 212], 'radius': 2.00},
    "Es": {'rgb': [179, 31, 212], 'radius': 2.00},
    "Fm": {'rgb': [179, 31, 186], 'radius': 2.00},
    "Md": {'rgb': [179, 13, 166], 'radius': 2.00},
    "No": {'rgb': [189, 13, 135], 'radius': 2.00},
    "Lr": {'rgb': [199, 0, 102], 'radius': 2.00},
    "Rf": {'rgb': [204, 0, 89], 'radius': 2.00},
    "Db": {'rgb': [209, 0, 79], 'radius': 2.00},
    "Sg": {'rgb': [217, 0, 69], 'radius': 2.00},
    "Bh": {'rgb': [224, 0, 56], 'radius': 2.00},
    "Hs": {'rgb': [230, 0, 46], 'radius': 2.00},
    "Mt": {'rgb': [235, 0, 38], 'radius': 2.00}
}

"""
    apply heuristics in case atom names are still unique, e.g. in CIF files
"""
def extractAtomName(atomname):
    name = atomname[:2]
    if name not in atomColors:
        name = atomname[:1]
        if name not in atomColors:
            name = atomname[0] + atomname[1].lower()
            if name not in atomColors:
                print("Could not look up atom: '" + atomname + "'")
                name = "Xx"
    return name

def color(name):
    rgb = atomColors.get(name, default)['rgb']
    rgb.append(255)
    return [rgb[0]/255.0, rgb[1]/255.0, rgb[2]/255.0, rgb[3]/255.0]


def radius(name):
    return atomColors.get(name, default)["radius"]

"""Expects Bio.PDB residues, returns ramachandran 'type'
If this is the last residue in a polypeptide, use None
for next_residue.

Return value is a string: "General", "Glycine", "Proline", or "Pre-Pro".
"""
def ramachandranType(residue, next_residue):
    if residue.resname.upper() == "GLY":
        return "Glycine"
    elif residue.resname.upper() == "PRO":
        return "Proline"
    elif next_residue is not None and next_residue.resname.upper() == "PRO":
        return "Pre-Pro"
    else:
        return "General"