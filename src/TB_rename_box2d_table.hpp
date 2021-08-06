{ "ClipVertex", "Object", { "", {}, ""}, 0, 0, {}, { "b2Vec2", "b2ContactID"} },
{ "Features", "Object", { "", {}, ""}, 0, 8, {}, { "int", "int", "int", "int", "b2ContactID"} },
{ "b2AABB", "Object", { "", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2"} },
{ "b2Body", "Object", { "b2Body", { "b2BodyDef", "b2World"}, ""}, 0, 54, { "b2MassData", "b2XForm", "uint", "uint", "uint", "uint", "uint", "uint", "uint", "uint", "uint"}, { "uint", "int", "int", "b2XForm", "b2Sweep", "b2Vec2", "Number", "b2Vec2", "Number", "b2World", "b2Body", "b2Body", "b2Shape", "int", "b2ControllerEdge", "int", "b2JointEdge", "b2ContactEdge", "Number", "Number", "Number", "Number", "Number", ""} },
{ "b2BodyDef", "Object", { "b2BodyDef", {}, ""}, 0, 0, {}, { "b2MassData", "", "b2Vec2", "Number", "Number", "Number", "Boolean", "Boolean", "Boolean", "Boolean"} },
{ "b2Bound", "Object", { "", {}, ""}, 0, 3, {}, { "uint", "b2Proxy", "uint"} },
{ "b2BoundValues", "Object", { "", {}, ""}, 0, 0, {}, { "Array", "Array"} },
{ "b2BoundaryListener", "Object", { "", {}, ""}, 0, 1, {}, {} },
{ "b2BroadPhase", "Object", { "b2BroadPhase", { "b2AABB", "b2PairCallback"}, ""}, 1, 15, { "Boolean", "uint", "uint"}, { "b2PairManager", "Array", "b2Proxy", "Array", "Array", "Array", "int", "b2AABB", "b2Vec2", "int", "uint"} },
{ "b2BuoyancyController", "Object", { "", {}, ""}, 0, 2, {}, { "b2Vec2", "Number", "Number", "b2Vec2", "Number", "Number", "Boolean", "Boolean", "b2Vec2"} },
{ "b2CircleContact", "Object", { "b2CircleContact", { "b2Shape", "b2Shape"}, ""}, 2, 2, { "b2ContactPoint"}, { "Array", "b2Manifold", "b2Manifold"} },
{ "b2CircleDef", "Object", { "b2CircleDef", {}, ""}, 0, 0, {}, { "b2Vec2", "Number"} },
{ "b2CircleShape", "Object", { "b2CircleShape", { "b2ShapeDef"}, ""}, 0, 9, {}, { "b2Vec2", "Number"} },
{ "b2Collision", "Object", { "", {}, ""}, 8, 0, { "uint", "b2Vec2"}, {} },
{ "b2Color", "Object", { "b2Color", { "Number", "Number", "Number"}, ""}, 0, 5, {}, { "uint", "uint", "uint"} },
{ "b2ConstantAccelController", "Object", { "", {}, ""}, 0, 1, {}, { "b2Vec2"} },
{ "b2ConstantForceController", "Object", { "", {}, ""}, 0, 1, {}, { "b2Vec2"} },
{ "b2Contact", "Object", { "b2Contact", { "b2Shape", "b2Shape"}, ""}, 4, 8, { "uint", "uint", "uint", "uint", "Array", "Boolean"}, { "uint", "b2Contact", "b2Contact", "b2ContactEdge", "b2ContactEdge", "b2Shape", "b2Shape", "int", "Number"} },
{ "b2ContactConstraint", "Object", { "b2ContactConstraint", {}, ""}, 0, 0, {}, { "Array", "b2Vec2", "b2Mat22", "b2Mat22", "b2Manifold", "b2Body", "b2Body", "Number", "Number", "int"} },
{ "b2ContactConstraintPoint", "Object", { "", {}, ""}, 0, 0, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "Number", "Number", "Number"} },
{ "b2ContactEdge", "Object", { "", {}, ""}, 0, 0, {}, { "b2Body", "b2Contact", "b2ContactEdge", "b2ContactEdge"} },
{ "b2ContactFilter", "Object", { "", {}, ""}, 0, 2, { "b2ContactFilter"}, {} },
{ "b2ContactID", "Object", { "b2ContactID", {}, ""}, 0, 4, {}, { "Features", "uint"} },
{ "b2ContactListener", "Object", { "", {}, ""}, 0, 4, {}, {} },
{ "b2ContactManager", "Object", { "b2ContactManager", {}, ""}, 0, 4, { "b2ContactPoint"}, { "b2World", "b2NullContact", "Boolean"} },
{ "b2ContactPoint", "Object", { "", {}, ""}, 0, 0, {}, { "b2Shape", "b2Shape", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "b2ContactID"} },
{ "b2ContactRegister", "Object", { "", {}, ""}, 0, 0, {}, { "Function", "Function", "Boolean"} },
{ "b2ContactResult", "Object", { "", {}, ""}, 0, 0, {}, { "b2Shape", "b2Shape", "b2Vec2", "b2Vec2", "Number", "Number", "b2ContactID"} },
{ "b2ContactSolver", "Object", { "b2ContactSolver", { "b2TimeStep", "Array", "int", ""}, ""}, 0, 4, {}, { "b2TimeStep", "", "Array", "int"} },
{ "b2Controller", "Object", { "", {}, ""}, 0, 8, {}, { "b2Controller", "b2Controller", "b2World"} },
{ "b2ControllerEdge", "Object", { "", {}, ""}, 0, 0, {}, { "b2Controller", "b2Body", "b2ControllerEdge", "b2ControllerEdge", "b2ControllerEdge", "b2ControllerEdge"} },
{ "b2DebugDraw", "Object", { "b2DebugDraw", {}, ""}, 0, 22, { "uint", "uint", "uint", "uint", "uint", "uint", "uint", "uint"}, { "uint", "Sprite", "Number", "Number", "Number", "Number", "Number"} },
{ "b2DestructionListener", "Object", { "", {}, ""}, 0, 2, {}, {} },
{ "b2Distance", "Object", { "", {}, ""}, 8, 0, { "Array", "Array", "Array", "b2Point", "int"}, {} },
{ "b2DistanceJoint", "Object", { "b2DistanceJoint", { "b2DistanceJointDef"}, ""}, 0, 7, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "Number", "Number", "Number"} },
{ "b2DistanceJointDef", "Object", { "b2DistanceJointDef", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2", "Number", "Number", "Number"} },
{ "b2EdgeAndCircleContact", "Object", { "b2EdgeAndCircleContact", { "b2Shape", "b2Shape"}, ""}, 2, 3, { "b2ContactPoint"}, { "Array", "b2Manifold", "b2Manifold"} },
{ "b2EdgeChainDef", "Object", { "b2EdgeChainDef", {}, ""}, 0, 0, {}, { "Array", "int", "Boolean"} },
{ "b2EdgeShape", "Object", { "b2EdgeShape", { "b2Vec2", "b2Vec2", "b2ShapeDef"}, ""}, 0, 24, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Boolean", "Boolean", "b2EdgeShape", "b2EdgeShape"} },
{ "b2FilterData", "Object", { "", {}, ""}, 0, 1, {}, { "uint", "uint", "int"} },
{ "b2GearJoint", "Object", { "b2GearJoint", { "b2GearJointDef"}, ""}, 0, 8, {}, { "b2Body", "b2Body", "b2RevoluteJoint", "b2PrismaticJoint", "b2RevoluteJoint", "b2PrismaticJoint", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Jacobian", "Number", "Number", "Number", "Number"} },
{ "b2GearJointDef", "Object", { "b2GearJointDef", {}, ""}, 0, 0, {}, { "b2Joint", "b2Joint", "Number"} },
{ "b2GravityController", "Object", { "", {}, ""}, 0, 1, {}, { "Number", "Boolean"} },
{ "b2Island", "Object", { "b2Island", { "int", "int", "int", "", "b2ContactListener"}, ""}, 0, 7, { "b2ContactResult"}, { "", "b2ContactListener", "Array", "Array", "Array", "int", "int", "int", "int", "int", "int"} },
{ "b2Jacobian", "Object", { "", {}, ""}, 0, 3, {}, { "b2Vec2", "Number", "b2Vec2", "Number"} },
{ "b2Joint", "Object", { "b2Joint", { "b2JointDef"}, ""}, 2, 14, { "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int"}, { "int", "b2Joint", "b2Joint", "b2JointEdge", "b2JointEdge", "b2Body", "b2Body", "Boolean", "Boolean", "", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number"} },
{ "b2JointDef", "Object", { "b2JointDef", {}, ""}, 0, 0, {}, { "int", "", "b2Body", "b2Body", "Boolean"} },
{ "b2JointEdge", "Object", { "", {}, ""}, 0, 0, {}, { "b2Body", "b2Joint", "b2JointEdge", "b2JointEdge"} },
{ "b2LineJoint", "Object", { "b2LineJoint", { "b2LineJointDef"}, ""}, 0, 20, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "b2Mat22", "b2Vec2", "Number", "Number", "Number", "Number", "Number", "Number", "Boolean", "Boolean", "int"} },
{ "b2LineJointDef", "Object", { "b2LineJointDef", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "Boolean", "Number", "Number", "Boolean", "Number", "Number"} },
{ "b2Manifold", "Object", { "b2Manifold", {}, ""}, 0, 2, {}, { "Array"} },
{ "b2ManifoldPoint", "Object", { "", {}, ""}, 0, 2, {}, { "b2Vec2", "b2Vec2", "Number", "Number", "Number", "b2ContactID"} },
{ "b2MassData", "Object", { "", {}, ""}, 0, 0, {}, { "Number", "b2Vec2", "Number"} },
{ "b2Mat22", "Object", { "b2Mat22", { "Number", "b2Vec2", "b2Vec2"}, ""}, 0, 11, {}, { "b2Vec2", "b2Vec2"} },
{ "b2Mat33", "Object", { "b2Mat33", { "b2Vec3", "b2Vec3", "b2Vec3"}, ""}, 0, 8, {}, { "b2Vec3", "b2Vec3", "b2Vec3"} },
{ "b2Math", "Object", { "", {}, ""}, 31, 0, { "b2Vec2", "b2Mat22", "b2XForm"}, {} },
{ "b2MouseJoint", "Object", { "b2MouseJoint", { "b2MouseJointDef"}, ""}, 0, 8, {}, { "b2Mat22", "b2Mat22", "b2Mat22", "b2Vec2", "b2Vec2", "b2Vec2", "b2Mat22", "b2Vec2", "Number", "Number", "Number", "Number", "Number"} },
{ "b2MouseJointDef", "Object", { "b2MouseJointDef", {}, ""}, 0, 0, {}, { "b2Vec2", "Number", "Number", "Number"} },
{ "b2NullContact", "Object", { "b2NullContact", {}, ""}, 0, 2, {}, {} },
{ "b2OBB", "Object", { "", {}, ""}, 0, 0, {}, { "b2Mat22", "b2Vec2", "b2Vec2"} },
{ "b2Pair", "Object", { "", {}, ""}, 0, 8, { "uint", "uint", "uint", "uint"}, { "", "b2Proxy", "b2Proxy", "b2Pair", "uint"} },
{ "b2PairCallback", "Object", { "", {}, ""}, 0, 2, {}, {} },
{ "b2PairManager", "Object", { "b2PairManager", {}, ""}, 0, 9, {}, { "b2BroadPhase", "b2PairCallback", "Array", "b2Pair", "int", "Array", "int"} },
{ "b2Point", "Object", { "", {}, ""}, 0, 2, {}, { "b2Vec2"} },
{ "b2PolyAndCircleContact", "Object", { "b2PolyAndCircleContact", { "b2Shape", "b2Shape"}, ""}, 2, 2, { "b2ContactPoint"}, { "Array", "b2Manifold", "b2Manifold"} },
{ "b2PolyAndEdgeContact", "Object", { "b2PolyAndEdgeContact", { "b2Shape", "b2Shape"}, ""}, 2, 3, { "b2ContactPoint"}, { "Array", "b2Manifold", "b2Manifold"} },
{ "b2PolygonContact", "Object", { "b2PolygonContact", { "b2Shape", "b2Shape"}, "void"}, 2, 2, { "b2ContactPoint"}, { "b2Manifold", "Array", "b2Manifold"} },
{ "b2PolygonDef", "Object", { "b2PolygonDef", {}, ""}, 0, 2, { "b2Mat22"}, { "Array", "int"} },
{ "b2PolygonShape", "Object", { "b2PolygonShape", { "b2ShapeDef"}, ""}, 2, 16, { "b2Mat22", "b2AABB", "b2AABB"}, { "b2Vec2", "b2Vec2", "b2OBB", "Array", "Array", "Array", "int"} },
{ "b2PrismaticJoint", "Object", { "b2PrismaticJoint", { "b2PrismaticJointDef"}, ""}, 0, 20, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "b2Mat33", "b2Vec3", "Number", "Number", "Number", "Number", "Number", "Number", "Boolean", "Boolean", "int"} },
{ "b2PrismaticJointDef", "Object", { "b2PrismaticJointDef", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Boolean", "Number", "Number", "Boolean", "Number", "Number"} },
{ "b2Proxy", "Object", { "", {}, ""}, 0, 1, {}, { "Array", "Array", "uint", "uint", "Dictionary", "b2Proxy", ""} },
{ "b2PulleyJoint", "Object", { "b2PulleyJoint", { "b2PulleyJointDef"}, ""}, 0, 12, { "Number"}, { "b2Body", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "int", "int", "int"} },
{ "b2PulleyJointDef", "Object", { "b2PulleyJointDef", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number", "Number", "Number"} },
{ "b2RevoluteJoint", "Object", { "b2RevoluteJoint", { "b2RevoluteJointDef"}, ""}, 0, 20, { "b2Vec2"}, { "b2Mat22", "b2Mat22", "b2Mat22", "b2Mat22", "b2Vec2", "b2Vec2", "b2Vec3", "Number", "b2Mat33", "Number", "Boolean", "Number", "Number", "Boolean", "Number", "Number", "Number", "int"} },
{ "b2RevoluteJointDef", "Object", { "b2RevoluteJointDef", {}, ""}, 0, 1, {}, { "b2Vec2", "b2Vec2", "Number", "Boolean", "Number", "Number", "Boolean", "Number", "Number"} },
{ "b2Segment", "Object", { "", {}, ""}, 0, 4, {}, { "b2Vec2", "b2Vec2"} },
{ "b2Settings", "Object", { "", {}, ""}, 3, 0, { "int", "Number", "int", "int", "Number", "Number", "Number", "int", "int", "int", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number", "Number"}, {} },
{ "b2Shape", "Object", { "b2Shape", { "b2ShapeDef"}, ""}, 2, 25, { "b2AABB", "b2AABB", "b2AABB", "int", "int", "int", "int", "int", "int", "int", "int"}, { "int", "b2Shape", "b2Body", "Number", "Number", "Number", "Number", "b2Proxy", "b2FilterData", "Boolean", ""} },
{ "b2ShapeDef", "Object", { "", {}, ""}, 0, 0, {}, { "int", "", "Number", "Number", "Number", "Boolean", "b2FilterData"} },
{ "b2Sweep", "Object", { "", {}, ""}, 0, 2, {}, { "b2Vec2", "b2Vec2", "b2Vec2", "Number", "Number", "Number"} },
{ "b2TensorDampingController", "Object", { "", {}, ""}, 0, 2, {}, { "b2Mat22", "Number"} },
{ "b2TimeOfImpact", "Object", { "", {}, ""}, 1, 0, { "b2Vec2", "b2Vec2", "b2XForm", "b2XForm"}, {} },
{ "b2TimeStep", "Object", { "", {}, ""}, 0, 0, {}, { "Number", "Number", "Number", "int", "int", "Boolean"} },
{ "b2Vec2", "Object", { "b2Vec2", { "Number", "Number"}, "void"}, 1, 19, {}, { "Number", "Number"} },
{ "b2Vec3", "Object", { "b2Vec3", { "Number", "Number", "Number"}, ""}, 0, 8, {}, { "Number", "Number", "Number"} },
{ "b2World", "Object", { "b2World", { "b2AABB", "b2Vec2", "Boolean"}, ""}, 0, 39, { "b2Color", "b2Color", "b2XForm", "Boolean", "Boolean"}, { "", "b2Segment", "b2Vec2", "", "", "Boolean", "b2BroadPhase", "b2ContactManager", "b2Body", "b2Joint", "b2Contact", "int", "int", "int", "b2Controller", "int", "b2Vec2", "Boolean", "b2Body", "b2DestructionListener", "b2BoundaryListener", "b2ContactFilter", "b2ContactListener", "b2DebugDraw", "Number"} },
