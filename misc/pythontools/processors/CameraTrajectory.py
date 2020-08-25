# Name: CameraTrajectory 

import math
import json

import inviwopy as ivw
import inviwopy.glm as glm

import numpy as np
import scipy.interpolate as ip

# https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.make_interp_spline.html
def interpolate(ctrlpts, steps = 100, order = 3):
	phi = np.linspace(0, 1, len(ctrlpts))
	spl = ip.make_interp_spline(x = phi, y = np.array(ctrlpts))
	phi_new = np.linspace(0, 1, len(ctrlpts)*steps)
	points = spl(phi_new)
	return np.array(points)

def rotateVec(m, v):
	return glm.vec3(m*glm.vec4(v, 1.0))

class CameraTrajectory(ivw.Processor):
	def __init__(self, id, name):
		ivw.Processor.__init__(self, id, name)

		self.outport = ivw.data.MeshOutport("outport")
		self.addOutport(self.outport, owner=False)

		self.ctrlpts = ivw.data.MeshOutport("ctrlpts")
		self.addOutport(self.ctrlpts, owner=False)

		self.frame = ivw.properties.IntProperty("frame", "frame", 0, 0, 159, 1)
		self.addProperty(self.frame, owner=False)		

		self.scale = ivw.properties.FloatProperty("scale", "scale", 1.0, 0.0, 100.0, 1.0)
		self.addProperty(self.scale, owner=False)

		self.add = ivw.properties.ButtonProperty("add", "Add Point")
		self.addProperty(self.add, owner=False)	

		self.camera = ivw.properties.CameraProperty("camera", "camera")
		self.addProperty(self.camera, owner=False)

		self.outCamera = ivw.properties.CameraProperty("outCamera", "outCamera")
		self.addProperty(self.outCamera, owner=False)
		self.outCamera.properties.lookFrom.minValue = glm.vec3(-10000.0)
		self.outCamera.properties.lookFrom.maxValue = glm.vec3(+10000.0)
		self.outCamera.properties.lookTo.minValue = glm.vec3(-10000.0)
		self.outCamera.properties.lookTo.maxValue = glm.vec3(+10000.0)
		self.outCamera.properties.lookFrom.semantics = ivw.properties.PropertySemantics.SpinBox
		self.outCamera.properties.lookTo.semantics = ivw.properties.PropertySemantics.SpinBox
		self.outCamera.properties.lookUp.semantics = ivw.properties.PropertySemantics.SpinBox
		self.outCamera.properties.fov.semantics = ivw.properties.PropertySemantics.SpinBox

		points = {"to" : [[1.0,0.0,0.0], [2.0,0.0,0.0], [3.0,0.0,0.0], [4.0,0.0,0.0]],
				  "from" : [[-100.-100,0.0,0.0], [100.0,-100.0,0.0], [100.0,100.0,0.0], [-100.0,100.0,0.0]],
				  "up" : [[0.0,0.0,1.0], [0.0,0.0,1.0], [0.0,0.0,1.0], [0.0,0.0,1.0]]}
		self.pts = ivw.properties.StringProperty("ctrlPrts", "ControlPoints", json.dumps(points, indent=4,))
		self.addProperty(self.pts, owner=False)
		self.pts.semantics = ivw.properties.PropertySemantics.Multiline

		self.fromPm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x, 1))
		self.toPm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x, 2))
		self.upPm = inviwopy.PickingMapper(self, 1, lambda x: self.callback(x, 3))
	
		self.fromTrack = []
		self.toTrack = []
		self.upTrack = []

		self.frame.onChange(self.updateFrame)
		self.add.onChange(self.addPoint)

		self.pts.onChange(self.doInterpolation)

	@staticmethod
	def processorInfo():
		return ivw.ProcessorInfo(
			classIdentifier = "org.inviwo.CameraTrajectory", 
			displayName = "Camera Trajectory",
			category = "Python",
			codeState = ivw.CodeState.Stable,
			tags = ivw.Tags.PY
		)

	def getProcessorInfo(self):
		return CameraTrajectory.processorInfo()

	def initializeResources(self):
		pass

	def addPoint(self):
		points = json.loads(self.pts.value)
		points["from"].append([0,0,0])
		points["to"].append([0,0,0])
		points["up"].append([0,0,1])
		self.pts.value = json.dumps(points, indent=4)

	def doInterpolation(self):
		points = json.loads(self.pts.value)
		if len(points["from"]) > 2:
			self.fromTrack = interpolate(points["from"])
			self.toTrack = interpolate(points["to"])
			self.upTrack = interpolate(points["up"])
			self.frame.maxValue = len(self.fromTrack)

			self.updateFrame()

	def updateFrame(self):
		currentFrame = self.frame.value
		if currentFrame < len(self.fromTrack):
			self.outCamera.properties.lookFrom.minValue = glm.vec3(-10000.0)
			self.outCamera.properties.lookFrom.maxValue = glm.vec3(+10000.0)
			self.outCamera.properties.lookTo.minValue = glm.vec3(-10000.0)
			self.outCamera.properties.lookTo.maxValue = glm.vec3(+10000.0)

			fromVec = glm.vec3(self.fromTrack[currentFrame])
			toVec = glm.vec3(self.toTrack[currentFrame])
			upVec = glm.vec3(self.upTrack[currentFrame])

			normal = toVec - fromVec
			right = glm.cross(normal, upVec)
			upVec = glm.normalize(glm.cross(right, normal))
			
			self.outCamera.lookFrom = fromVec
			self.outCamera.lookTo = toVec
			self.outCamera.lookUp = upVec


	def ctrlToMesh(self):
		ctrlpts = ivw.data.Mesh()
		position = []
		color = []
		radius = []
		picking = []
		index = []

		points = json.loads(self.pts.value)

		self.fromPm.resize(len(points["from"]))
		for i, p in enumerate(points["from"]):
			position.append(p)
			color.append([0,1,0,1])
			radius.append(self.scale.value * 0.25)
			picking.append(self.fromPm.pickingId(i))
			index.append(i)

		count = len(index)
		self.toPm.resize(len(points["to"]))
		for i, p in enumerate(points["to"]):
			position.append(p)
			color.append([0,0,1,1])
			radius.append(self.scale.value * 0.25)
			picking.append(self.toPm.pickingId(i))
			index.append(count + i)

		count = len(index)
		self.upPm.resize(len(points["up"]))
		for i, [u, f] in enumerate(zip(points["up"], points["from"])):
			position.append([f[0] + self.scale.value * u[0], f[1] +  self.scale.value * u[1], f[2] + self.scale.value * u[2]])
			color.append([1,0,1,1])
			radius.append(self.scale.value * 0.15)
			picking.append(self.upPm.pickingId(i))
			index.append(count + i)

		ctrlpts.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
			numpy.array(position).astype(numpy.float32)))
		ctrlpts.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
			numpy.array(color).astype(numpy.float32)))
		ctrlpts.addBuffer(ivw.data.BufferType.RadiiAttrib, ivw.data.BufferFLOAT32(
			numpy.array(radius).astype(numpy.float32)))
		ctrlpts.addBuffer(ivw.data.BufferType.PickingAttrib, ivw.data.BufferUINT32(
			numpy.array(picking).astype(numpy.uint32)))

		pointsNone = ivw.data.MeshInfo(ivw.data.DrawType.Points, ivw.data.ConnectivityType.None_)
		ctrlpts.addIndices(pointsNone, ivw.data.IndexBufferUINT32(numpy.array(index).astype(numpy.uint32)))

		return ctrlpts


	def pathToMesh(self):
		position = []
		color = []
		viewIndex = []
		upIndex = []
		toIndex = []
		fromIndex = []
		upPathIndex = []
				
		scale = self.scale.value
		count = 0

		points = json.loads(self.pts.value)
		
		vert = 3
		for lookFrom, lookTo, lookUp in zip(self.fromTrack, self.toTrack, self.upTrack):
			position.append(lookTo)
			position.append(lookFrom)
			position.append([lookFrom[0] + scale * lookUp[0], lookFrom[1] + scale * lookUp[1], lookFrom[2]+ scale * lookUp[2]])
			color.append([1.0, 0.1, 1.0, 1.0])
			color.append([1.0, 1.0, 0.0, 1.0])
			color.append([0.0, 1.0, 1.0, 1.0])
			if count % 10 == 0:
				viewIndex.append(count * vert + 0)
				viewIndex.append(count * vert + 1)
				upIndex.append(count * vert + 1)
				upIndex.append(count * vert + 2)
			toIndex.append(count * vert + 0)
			fromIndex.append(count * vert + 1)
			upPathIndex.append(count * vert + 2)
			count += 1

		mesh = ivw.data.Mesh()

		mesh.addBuffer(ivw.data.BufferType.PositionAttrib, ivw.data.BufferVec3FLOAT32(
			numpy.array(position).astype(numpy.float32)))
		mesh.addBuffer(ivw.data.BufferType.ColorAttrib, ivw.data.BufferVec4FLOAT32(
			numpy.array(color).astype(numpy.float32)))
		
		lineNone = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.None_)
		lineStrip = ivw.data.MeshInfo(ivw.data.DrawType.Lines, ivw.data.ConnectivityType.Strip)
		mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(numpy.array(viewIndex).astype(numpy.uint32)))
		mesh.addIndices(lineNone, ivw.data.IndexBufferUINT32(numpy.array(upIndex).astype(numpy.uint32)))
		mesh.addIndices(lineStrip, ivw.data.IndexBufferUINT32(numpy.array(toIndex).astype(numpy.uint32)))
		mesh.addIndices(lineStrip, ivw.data.IndexBufferUINT32(numpy.array(fromIndex).astype(numpy.uint32)))
		mesh.addIndices(lineStrip, ivw.data.IndexBufferUINT32(numpy.array(upPathIndex).astype(numpy.uint32)))
		return mesh

	def process(self):	
		self.outport.setData(self.pathToMesh())
		self.ctrlpts.setData(self.ctrlToMesh())

	def callback(self, pickevent, type):
		if pickevent.state == ivw.PickingState.Updated:
			i = pickevent.pickedId
			points = json.loads(self.pts.value)
			pos = points["from"][i] if type == 1 else points["to"][i]
			pickevent.setToolTip(f"{type} {i} {pos}")
		else:
			pickevent.setToolTip("")

		# and ivw.PickingPressItem.Primary in pickevent.pressItems
		if (pickevent.pressState == ivw.PickingPressState.Move 
			and ivw.PickingPressItem.Primary in pickevent.pressItems): 
			points = json.loads(self.pts.value)
	
			delta = pickevent.getWorldSpaceDeltaAtPressDepth(self.camera.camera)
			print(f"drag {delta}")

			upPos = glm.dvec3(points["up"][pickevent.pickedId])

			if type == 1:
				points["from"][pickevent.pickedId][0] += delta.x
				points["from"][pickevent.pickedId][1] += delta.y
				points["from"][pickevent.pickedId][2] += delta.z
			elif type == 2:
				points["to"][pickevent.pickedId][0] += delta.x
				points["to"][pickevent.pickedId][1] += delta.y
				points["to"][pickevent.pickedId][2] += delta.z
			else:
				upPos += delta / self.scale.value
				
			normal = glm.dvec3(points["to"][pickevent.pickedId]) - glm.dvec3(points["from"][pickevent.pickedId])
			upPos = upPos - normal * (glm.dot(upPos, normal) / glm.length2(normal))
			upPos = glm.normalize(upPos)
			points["up"][pickevent.pickedId][0] = upPos.x
			points["up"][pickevent.pickedId][1] = upPos.y
			points["up"][pickevent.pickedId][2] = upPos.z

			pickevent.markAsUsed()
			self.pts.value = json.dumps(points, indent=4,)
			self.invalidate(ivw.properties.InvalidationLevel.InvalidOutput)
