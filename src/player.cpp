#include "player.h"

namespace redc
{
  Player_Controller::Player_Controller() : inited_(false),
                                           ghost_(),
                                           shape_(.25f, 1.8f) {}

  // They are expected to add the action to the dynamics world and then this
  // will be called, then we init then we start
  void Player_Controller::updateAction(btCollisionWorld* world, btScalar dt)
  {
    if(!inited_)
    {
      btTransform trans;
      trans.setIdentity();
      trans.setOrigin(btVector3(0.0f, .5f, 0.0f));
      ghost_.setWorldTransform(trans);
      ghost_.setCollisionShape(&shape_);
      world->addCollisionObject(&ghost_);
      ghost_.setCollisionFlags(ghost_.getCollisionFlags() |
                               btCollisionObject::CF_KINEMATIC_OBJECT);
      ghost_.setActivationState(DISABLE_DEACTIVATION);
      inited_ = true;
      return;
    }

    // Recover from intersection
    btVector3 min_aabb, max_aabb;
    shape_.getAabb(ghost_.getWorldTransform(), min_aabb, max_aabb);
    world->getBroadphase()->setAabb(ghost_.getBroadphaseHandle(), min_aabb,
                                    max_aabb, world->getDispatcher());
    bool penetration = false;
    world->getDispatcher()->dispatchAllCollisionPairs(
            ghost_.getOverlappingPairCache(),
            world->getDispatchInfo(),
            world->getDispatcher());

    auto cur_pos = ghost_.getWorldTransform().getOrigin();

    btScalar maxPen = btScalar(0.0);

    auto xform = ghost_.getWorldTransform();

    btVector3 local_dpos;

    if(input_ref_->forward)
    {
      local_dpos.setZ(-.1f);
    }
    if(input_ref_->backward)
    {
      local_dpos.setZ(+.1f);
    }
    if(input_ref_->strafe_left)
    {
      local_dpos.setX(-.1f);
    }
    if(input_ref_->strafe_right)
    {
      local_dpos.setX(+.1f);
    }
    auto orig = xform.getOrigin();
    orig += btMatrix3x3(xform.getRotation()) * local_dpos;
    xform.setOrigin(orig);

    ghost_.setWorldTransform(xform);
  }

  void Player_Controller::apply_delta_yaw(double yaw)
  {
    auto xform = ghost_.getWorldTransform();
    auto rot = xform.getRotation();
    rot *= btQuaternion(-yaw, 0.0f, 0.0f);
    xform.setRotation(rot);
    ghost_.setWorldTransform(xform);
  }
  void Player_Controller::apply_delta_pitch(double pitch)
  {
    // We don't care about pitch as of now from a physics standpoint
    /*
    auto xform = ghost_.getWorldTransform();
    auto rot = xform.getRotation();
    rot *= btQuaternion(0.0f, pitch, 0.0f);
    ghost_.setWorldTransform(xform);
    */
  }
}
