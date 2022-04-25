
#include "as2_motion_command_handlers/trajectory_motion.hpp"

namespace as2
{
    namespace motionCommandsHandlers
    {
        TrajectoryMotion::TrajectoryMotion(as2::Node *node_ptr) : BasicMotionCommandsHandler(node_ptr)
        {
            desired_control_mode_.yaw_mode = as2_msgs::msg::ControlMode::NONE;
            desired_control_mode_.control_mode = as2_msgs::msg::ControlMode::TRAJECTORY;
            desired_control_mode_.reference_frame = as2_msgs::msg::ControlMode::LOCAL_ENU_FRAME;
        };

        bool TrajectoryMotion::sendTrajectoryCommandWithYawAngle(
            const float &x, const float &y, const float &z, const float &yaw_angle,
            const float &vx, const float &vy, const float &vz,
            const float &ax, const float &ay, const float &az)
        {
            desired_control_mode_.yaw_mode = as2_msgs::msg::ControlMode::YAW_ANGLE;
            /*
            Matrix:
            | x_ref_x   | v_ref_x   | a_ref_x   |
            | x_ref_y   | v_ref_y   | a_ref_y   |
            | x_ref_z   | v_ref_z   | a_ref_z   |
            | x_ref_yaw | v_ref_yaw | a_ref_yaw |
            */
            this->command_trajectory_msg_.positions[0] = x;
            this->command_trajectory_msg_.velocities[0] = vx;
            this->command_trajectory_msg_.accelerations[0] = ax;

            this->command_trajectory_msg_.positions[1] = y;
            this->command_trajectory_msg_.velocities[1] = vy;
            this->command_trajectory_msg_.accelerations[1] = ay;

            this->command_trajectory_msg_.positions[2] = z;
            this->command_trajectory_msg_.velocities[2] = vz;
            this->command_trajectory_msg_.accelerations[2] = az;

            this->command_trajectory_msg_.positions[3] = yaw_angle;
            this->command_trajectory_msg_.velocities[3] = 0.0;
            this->command_trajectory_msg_.accelerations[3] = 0.0;

            return this->sendCommand();
        };

        bool TrajectoryMotion::sendTrajectoryCommandWithYawSpeed(
            const float &x, const float &y, const float &z,
            const float &vx, const float &vy, const float &vz, const float &yaw_speed,
            const float &ax, const float &ay, const float &az)
        {
            desired_control_mode_.yaw_mode = as2_msgs::msg::ControlMode::YAW_SPEED;
            /*
            Matrix:
            | x_ref_x   | v_ref_x   | a_ref_x   |
            | x_ref_y   | v_ref_y   | a_ref_y   |
            | x_ref_z   | v_ref_z   | a_ref_z   |
            | x_ref_yaw | v_ref_yaw | a_ref_yaw |
            */
            this->command_trajectory_msg_.positions[0] = x;
            this->command_trajectory_msg_.velocities[0] = vx;
            this->command_trajectory_msg_.accelerations[0] = ax;

            this->command_trajectory_msg_.positions[1] = y;
            this->command_trajectory_msg_.velocities[1] = vy;
            this->command_trajectory_msg_.accelerations[1] = ay;

            this->command_trajectory_msg_.positions[2] = z;
            this->command_trajectory_msg_.velocities[2] = vz;
            this->command_trajectory_msg_.accelerations[2] = az;

            this->command_trajectory_msg_.positions[3] = 0.0;
            this->command_trajectory_msg_.velocities[3] = yaw_speed;
            this->command_trajectory_msg_.accelerations[3] = 0.0;

            return this->sendCommand();
        };
    } // namespace motionCommandsHandlers
} // namespace as2
