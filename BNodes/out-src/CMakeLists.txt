cmake_minimum_required(VERSION 2.6)

project(
nodetree_Gamekit
)

MESSAGE(STATUS "Generated Logic: ${CMAKE_CURRENT_SOURCE_DIR}")
MESSAGE(STATUS "gkOgreKITInclude: ${OGREKIT_INCLUDE}")

include_directories(
	${OGREKIT_INCLUDE}

	src-nodes/NodesImpl

	src-gen/Generated

)

set(
	FILES

	src-nodes/NodesImpl/gkPickRayNodeImpl.h

	src-nodes/NodesImpl/gkPickRayNodeImpl.cpp

	src-nodes/NodesImpl/gkTestNodeImpl.h

	src-nodes/NodesImpl/gkTestNodeImpl.cpp

	src-nodes/NodesImpl/gkMessageSendImpl.h

	src-nodes/NodesImpl/gkMessageSendImpl.cpp

	src-nodes/NodesImpl/gkMessageGetImpl.h

	src-nodes/NodesImpl/gkMessageGetImpl.cpp

	src-nodes/NodesImpl/gkObjectManipulatorImpl.h

	src-nodes/NodesImpl/gkObjectManipulatorImpl.cpp

	src-nodes/NodesImpl/gkCharacterNodeImpl.h

	src-nodes/NodesImpl/gkCharacterNodeImpl.cpp

	src-nodes/NodesImpl/gkSequenceImpl.h

	src-nodes/NodesImpl/gkSequenceImpl.cpp

	src-nodes/NodesImpl/gkSeqFinishedImpl.h

	src-nodes/NodesImpl/gkSeqFinishedImpl.cpp

	src-nodes/NodesImpl/gkSeqTimeImpl.h

	src-nodes/NodesImpl/gkSeqTimeImpl.cpp

	src-nodes/NodesImpl/gkMotionNodeImpl.h

	src-nodes/NodesImpl/gkMotionNodeImpl.cpp

	src-nodes/NodesImpl/gkMouseNodeImpl.h

	src-nodes/NodesImpl/gkMouseNodeImpl.cpp

	src-nodes/NodesImpl/gkBoolNodeImpl.h

	src-nodes/NodesImpl/gkBoolNodeImpl.cpp

	src-nodes/NodesImpl/gkMathNodeImpl.h

	src-nodes/NodesImpl/gkMathNodeImpl.cpp

	src-nodes/NodesImpl/gkTimerNodeImpl.h

	src-nodes/NodesImpl/gkTimerNodeImpl.cpp

	src-nodes/NodesImpl/gkObjectNodeImpl.h

	src-nodes/NodesImpl/gkObjectNodeImpl.cpp

	src-nodes/NodesImpl/gkObjectDataImpl.h

	src-nodes/NodesImpl/gkObjectDataImpl.cpp

	src-nodes/NodesImpl/gkAccelerometerImpl.h

	src-nodes/NodesImpl/gkAccelerometerImpl.cpp

	src-nodes/NodesImpl/gkVectorDecompImpl.h

	src-nodes/NodesImpl/gkVectorDecompImpl.cpp

	src-nodes/NodesImpl/gkNodeTreeNodeImpl.h

	src-nodes/NodesImpl/gkNodeTreeNodeImpl.cpp

	src-nodes/NodesImpl/gkStateMachineImpl.h

	src-nodes/NodesImpl/gkStateMachineImpl.cpp

	src-nodes/NodesImpl/gkStateImpl.h

	src-nodes/NodesImpl/gkStateImpl.cpp

	src-nodes/NodesImpl/gkStateTransitionImpl.h

	src-nodes/NodesImpl/gkStateTransitionImpl.cpp

	src-nodes/NodesImpl/gkStateMachineRefImpl.h

	src-nodes/NodesImpl/gkStateMachineRefImpl.cpp

	src-nodes/NodesImpl/gkStateRefImpl.h

	src-nodes/NodesImpl/gkStateRefImpl.cpp

	src-nodes/NodesImpl/gkStateManipulatorImpl.h

	src-nodes/NodesImpl/gkStateManipulatorImpl.cpp

	src-nodes/NodesImpl/gkPropertyImpl.h

	src-nodes/NodesImpl/gkPropertyImpl.cpp

	src-nodes/NodesImpl/gkPropertyGetImpl.h

	src-nodes/NodesImpl/gkPropertyGetImpl.cpp

	src-nodes/NodesImpl/gkNoOpImpl.h

	src-nodes/NodesImpl/gkNoOpImpl.cpp

	src-nodes/NodesImpl/gkTemplaterNodeImpl.h

	src-nodes/NodesImpl/gkTemplaterNodeImpl.cpp

	src-nodes/NodesImpl/gkScreenActionImpl.h

	src-nodes/NodesImpl/gkScreenActionImpl.cpp

	src-nodes/NodesImpl/gkElementImpl.h

	src-nodes/NodesImpl/gkElementImpl.cpp

	src-nodes/NodesImpl/gkAnimationDefinitionImpl.h

	src-nodes/NodesImpl/gkAnimationDefinitionImpl.cpp

	src-nodes/NodesImpl/gkAnimationPlayerImpl.h

	src-nodes/NodesImpl/gkAnimationPlayerImpl.cpp

	src-nodes/NodesImpl/gkAnimationTriggerImpl.h

	src-nodes/NodesImpl/gkAnimationTriggerImpl.cpp

	src-nodes/NodesImpl/gkCollisionNodeImpl.h

	src-nodes/NodesImpl/gkCollisionNodeImpl.cpp

	src-nodes/NodesImpl/gkFileNodeImpl.h

	src-nodes/NodesImpl/gkFileNodeImpl.cpp

	src-nodes/NodesImpl/gkLuaNodeImpl.h

	src-nodes/NodesImpl/gkLuaNodeImpl.cpp

	src-nodes/NodesImpl/gkAdditionNodeImpl.h

	src-nodes/NodesImpl/gkAdditionNodeImpl.cpp

	src-nodes/NodesImpl/gkPrintNodeImpl.h

	src-nodes/NodesImpl/gkPrintNodeImpl.cpp

	src-nodes/NodesImpl/gkKeyNodeImpl.h

	src-nodes/NodesImpl/gkKeyNodeImpl.cpp

	src-nodes/NodesImpl/gkMouseButtonImpl.h

	src-nodes/NodesImpl/gkMouseButtonImpl.cpp

	src-nodes/NodesImpl/gkDeltaNodeImpl.h

	src-nodes/NodesImpl/gkDeltaNodeImpl.cpp

	src-nodes/NodesImpl/gkTypeColorImpl.h

	src-nodes/NodesImpl/gkTypeColorImpl.cpp

	src-nodes/NodesImpl/gkTypeIntImpl.h

	src-nodes/NodesImpl/gkTypeIntImpl.cpp

	src-nodes/NodesImpl/gkTypeFloatImpl.h

	src-nodes/NodesImpl/gkTypeFloatImpl.cpp

	src-nodes/NodesImpl/gkTypeBoolImpl.h

	src-nodes/NodesImpl/gkTypeBoolImpl.cpp

	src-nodes/NodesImpl/gkPBSMaterialImpl.h

	src-nodes/NodesImpl/gkPBSMaterialImpl.cpp

	src-nodes/NodesImpl/gkPBSSlotImpl.h

	src-nodes/NodesImpl/gkPBSSlotImpl.cpp

	src-nodes/NodesImpl/gkPBSTex_AlbedoImpl.h

	src-nodes/NodesImpl/gkPBSTex_AlbedoImpl.cpp

	src-nodes/NodesImpl/gkPBSTex_EnvImpl.h

	src-nodes/NodesImpl/gkPBSTex_EnvImpl.cpp

	src-nodes/NodesImpl/gkPBSTex_NormalImpl.h

	src-nodes/NodesImpl/gkPBSTex_NormalImpl.cpp

	src-nodes/NodesImpl/gkPBSTex_F0Impl.h

	src-nodes/NodesImpl/gkPBSTex_F0Impl.cpp

	src-nodes/NodesImpl/gkEngineNodeImpl.h

	src-nodes/NodesImpl/gkEngineNodeImpl.cpp

	src-nodes/NodesImpl/gkIfNodeImpl.h

	src-nodes/NodesImpl/gkIfNodeImpl.cpp

	src-nodes/NodesImpl/gkSoundNodeImpl.h

	src-nodes/NodesImpl/gkSoundNodeImpl.cpp

	src-nodes/NodesImpl/gkJoystickNodeImpl.h

	src-nodes/NodesImpl/gkJoystickNodeImpl.cpp

	src-nodes/NodesImpl/gkJoystickButtonImpl.h

	src-nodes/NodesImpl/gkJoystickButtonImpl.cpp

	src-nodes/NodesImpl/gkVehicleNodeImpl.h

	src-nodes/NodesImpl/gkVehicleNodeImpl.cpp

	src-nodes/NodesImpl/gkVehicleGearboxImpl.h

	src-nodes/NodesImpl/gkVehicleGearboxImpl.cpp

	src-nodes/NodesImpl/gkVehicleGearImpl.h

	src-nodes/NodesImpl/gkVehicleGearImpl.cpp

	src-nodes/NodesImpl/gkVehicleWheelImpl.h

	src-nodes/NodesImpl/gkVehicleWheelImpl.cpp

	src-nodes/NodesImpl/gkCameraNodeImpl.h

	src-nodes/NodesImpl/gkCameraNodeImpl.cpp

	src-gen/Generated/gsGamekit.h

	src-gen/Generated/gsGamekit.cpp

	src-gen/Generated/gkGamekit.h

	src-gen/Generated/gkGamekit.cpp

)

add_library(${PROJECT_NAME} ${FILES})
