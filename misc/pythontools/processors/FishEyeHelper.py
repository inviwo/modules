# Name: FishEyeHelper 

#& 'C:\Program Files\ffmpeg\bin\ffmpeg.exe' -framerate 30 -i isabel/fisheye_2K_R_%06d.png -framerate 30 -i isabel/fisheye_2K_L_%06d.png -c:v libx265 -filter_complex hstack isabel2.mp4

import inviwopy as ivw
import inviwopy.glm as glm

import math

class FishEyeHelper(ivw.Processor):
	def __init__(self, id, name):
		ivw.Processor.__init__(self, id, name)

		self.current = ivw.properties.OptionPropertyInt("current", "Current", [
			ivw.properties.IntOption("left_left", "left_left", 0),
			ivw.properties.IntOption("left_right", "left_right", 1),
			ivw.properties.IntOption("left_top", "left_top", 2),
			ivw.properties.IntOption("left_bottom", "left_bottom", 3),
			ivw.properties.IntOption("right_left", "right_left", 4),
			ivw.properties.IntOption("right_right", "right_right", 5),
			ivw.properties.IntOption("right_top", "right_top", 6),
			ivw.properties.IntOption("right_bottom", "right_bottom", 7),
			], 0)
		self.addProperty(self.current)

		self.separation = ivw.properties.FloatProperty("offset", "Eye Separation", 0.01, 0.0, 0.1)
		self.addProperty(self.separation, owner=False)

		self.tilt = ivw.properties.FloatProperty("tilt", "View Tilt", 27, -90, 90)
		self.addProperty(self.tilt, owner=False)

		self.master = ivw.properties.CameraProperty("master", "master")
		self.addProperty(self.master, owner=False)

		self.camera = ivw.properties.CameraProperty("camera", "camera")
		self.addProperty(self.camera, owner=False)
		
		self.master.onChange(self.updateCamera)
		self.current.onChange(self.updateCamera)
		self.separation.onChange(self.updateCamera)
		self.tilt.onChange(self.updateCamera)
	
		self.camera.properties.cameraType.value = "SkewedPerspectiveCamera"
		self.camera.properties.lookFrom.minValue = glm.vec3(-10000.0)
		self.camera.properties.lookFrom.maxValue = glm.vec3(+10000.0)
		self.camera.properties.lookTo.minValue = glm.vec3(-10000.0)
		self.camera.properties.lookTo.maxValue = glm.vec3(+10000.0)
		self.camera.properties.offset.minValue = glm.vec2(-10000.0)
		self.camera.properties.offset.maxValue = glm.vec2(+10000.0)
		self.camera.properties.fov.value = 90

		self.camera.properties.lookFrom.semantics = ivw.properties.PropertySemantics.SpinBox
		self.camera.properties.lookTo.semantics = ivw.properties.PropertySemantics.SpinBox
		self.camera.properties.lookUp.semantics = ivw.properties.PropertySemantics.SpinBox
		self.camera.properties.fov.semantics = ivw.properties.PropertySemantics.SpinBox
		self.camera.properties.offset.semantics = ivw.properties.PropertySemantics.SpinBox
		self.camera.readOnly = True

	def getRotation(self):
		viewDir = self.master.lookTo - self.master.lookFrom
		rightDir = glm.cross(viewDir, self.master.lookUp)

		tilt = glm.rotate(glm.mat4(1.0), self.tilt.value*math.pi/180, rightDir)
		leftRot = glm.rotate(tilt, math.pi/4, self.master.lookUp)

		if self.current.selectedValue == 0 or self.current.selectedValue == 4:
			return leftRot
		if self.current.selectedValue == 1 or self.current.selectedValue == 5:
			return glm.rotate(leftRot, -math.pi/2, self.master.lookUp)
		if self.current.selectedValue == 2 or self.current.selectedValue == 6:
			return glm.rotate(leftRot, math.pi/2, rightDir)
		if self.current.selectedValue == 3 or self.current.selectedValue == 7:
			return glm.rotate(leftRot, -math.pi/2, rightDir)

	def getOffset(self):
		offset = 0.005
		if self.current.selectedValue < 4:
			return -self.separation.value / 2
		else:
			return self.separation.value / 2


	def updateCamera(self):
		def rotateVec(m, v):
			return glm.vec3(m*glm.vec4(v, 1.0))
	
		self.camera.near.minValue = self.master.near.minValue
		self.camera.near.maxValue = self.master.near.maxValue
		self.camera.far.minValue = self.master.far.minValue
		self.camera.far.maxValue = self.master.far.maxValue
		self.camera.nearPlane = self.master.nearPlane
		self.camera.farPlane = self.master.farPlane
		self.camera.properties.fov.value = 90

		rot = self.getRotation()
		offset = self.getOffset()
		viewDir = self.master.lookTo - self.master.lookFrom
		
		self.camera.lookFrom = self.master.lookFrom
		self.camera.lookTo = self.master.lookFrom + rotateVec(rot, viewDir)
		self.camera.lookUp = rotateVec(rot, self.master.lookUp)

		self.camera.properties.offset.value = glm.vec2(offset,0)

	@staticmethod
	def processorInfo():
		return ivw.ProcessorInfo(
			classIdentifier = "org.inviwo.FishEyeHelper", 
			displayName = "FishEye Helper",
			category = "Python",
			codeState = ivw.CodeState.Stable,
			tags = ivw.Tags.PY
		)

	def getProcessorInfo(self):
		return FishEyeHelper.processorInfo()

	def initializeResources(self):
		pass

	def process(self):
		pass
