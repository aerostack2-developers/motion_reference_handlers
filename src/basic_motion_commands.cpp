#include "as2_motion_command_handlers/basic_motion_commands.hpp"

#include <as2_core/names/topics.hpp>

namespace as2
{
    namespace motionCommandsHandlers
    {
        BasicMotionCommandsHandler::BasicMotionCommandsHandler(as2::Node *as2_ptr) : node_ptr_(as2_ptr)
        {
            if (number_of_instances_ == 0)
            {
                // Publisher
                command_traj_pub_ = node_ptr_->create_publisher<trajectory_msgs::msg::JointTrajectoryPoint>(
                    as2_names::topics::motion_reference::trajectory, as2_names::topics::motion_reference::qos);

                command_twist_pub_ = node_ptr_->create_publisher<geometry_msgs::msg::TwistStamped>(
                    as2_names::topics::motion_reference::twist, as2_names::topics::motion_reference::qos);

                command_pose_pub_ = node_ptr_->create_publisher<geometry_msgs::msg::PoseStamped>(
                    as2_names::topics::motion_reference::pose, as2_names::topics::motion_reference::qos);

                // Subscriber
                controller_info_sub_ = node_ptr_->create_subscription<as2_msgs::msg::ControllerInfo>(
                    as2_names::topics::motion_reference::info, as2_names::topics::motion_reference::qos_info,
                    [](const as2_msgs::msg::ControllerInfo::SharedPtr msg)
                    {
                        BasicMotionCommandsHandler::current_mode_ = msg->current_control_mode;
                    });

                // Service client
                set_mode_client_ = node_ptr_->create_client<as2_msgs::srv::SetControllerControlMode>(
                    node_ptr_->generate_global_name(as2_names::services::motion_reference::setcontrolmode));
            }

            number_of_instances_++;
            RCLCPP_INFO(node_ptr_->get_logger(),
                        "There are %d instances of BasicMotionCommandsHandler created",
                        number_of_instances_);
        };

        BasicMotionCommandsHandler::~BasicMotionCommandsHandler()
        {
            number_of_instances_--;
            if (number_of_instances_ == 0 && node_ptr_ != nullptr)
            {
                RCLCPP_INFO(node_ptr_->get_logger(), "Deleting node_ptr_");
                set_mode_client_.reset();
                controller_info_sub_.reset();
            }
        };

        bool BasicMotionCommandsHandler::sendCommand()
        {
            static auto last_time = this->node_ptr_->now();

            setControlMode();

            if (this->current_mode_ != desired_control_mode_)
            {
                if (!setMode(desired_control_mode_))
                {
                    RCLCPP_ERROR(node_ptr_->get_logger(), "Cannot set control mode");
                    return false;
                }
            }

            publishCommands();
            return true;
        };

        void BasicMotionCommandsHandler::publishCommands()
        {
            rclcpp::Time stamp = node_ptr_->now();

            switch (current_mode_.control_mode)
            {
            case as2_msgs::msg::ControllerControlMode::HOVER_MODE:
                /* TODO */
                break;

            case as2_msgs::msg::ControllerControlMode::TRAJECTORY_MODE:
                command_traj_pub_->publish(command_trajectory_msg_);
                break;

            case as2_msgs::msg::ControllerControlMode::SPEED_MODE:
                command_twist_msg_.header.stamp = stamp;
                command_twist_msg_.header.frame_id = "odom";
                command_twist_pub_->publish(command_twist_msg_);
                break;
            case as2_msgs::msg::ControllerControlMode::UNSET:
                // TODO
                break;
            default:
                RCLCPP_WARN_ONCE(node_ptr_->get_logger(), "Unknown control mode");
                break;
            }

            switch (current_mode_.yaw_mode)
            {
            case as2_msgs::msg::ControllerControlMode::YAW_ANGLE:
                // TODO
                break;
            case as2_msgs::msg::ControllerControlMode::YAW_SPEED:
                // TODO
                break;
            case as2_msgs::msg::ControllerControlMode::NONE:
                // TODO
                break;
            default:
                RCLCPP_WARN_ONCE(node_ptr_->get_logger(), "Unknown yaw control mode");
                break;
            }
        }

        bool BasicMotionCommandsHandler::setMode(const as2_msgs::msg::ControllerControlMode& mode) {

          RCLCPP_INFO(node_ptr_->get_logger(), "Setting control mode to %d", mode.control_mode);
          auto request = std::make_shared<as2_msgs::srv::SetControllerControlMode::Request>();
          request->control_mode = mode;

          // RCLCPP_INFO(node_ptr_->get_logger(), "waiting for_service");
          // RCLCPP_INFO(node_ptr_->get_logger(), "ptr address=  %x", set_mode_client_.get());
          while (!set_mode_client_->wait_for_service(std::chrono::seconds(1))) {
            // RCLCPP_INFO(node_ptr_->get_logger(), "waiting for service ok");
            if (!rclcpp::ok()) {
              RCLCPP_ERROR(node_ptr_->get_logger(), "Interrupted while waiting for the service. Exiting.");
              return false;
            }
            RCLCPP_INFO(node_ptr_->get_logger(), "service not available, waiting again...");
          }
          // RCLCPP_INFO(node_ptr_->get_logger(), "service available");

          auto result = set_mode_client_->async_send_request(request);

          if (rclcpp::spin_until_future_complete(node_ptr_->get_node_base_interface(), result,
                                                 std::chrono::seconds(1)) ==
              rclcpp::executor::FutureReturnCode::SUCCESS) {
            RCLCPP_INFO(node_ptr_->get_logger(), "Controller Control Mode changed sucessfully");
            current_mode_ = mode;  // set current_mode_ to the new mode to avoid issues while current_mode_
                                   // is not updated
          } else {
            RCLCPP_ERROR(node_ptr_->get_logger(),
                         " Controller Control Mode was not able to be settled sucessfully");
            return false;
          }
          // RCLCPP_INFO(node_ptr_->get_logger(), "service called correctly");
          return true;
        };

        

        int BasicMotionCommandsHandler::number_of_instances_ = 0;
        rclcpp::Client<as2_msgs::srv::SetControllerControlMode>::SharedPtr
            BasicMotionCommandsHandler::set_mode_client_ = nullptr;
        rclcpp::Subscription<as2_msgs::msg::ControllerInfo>::SharedPtr
            BasicMotionCommandsHandler::controller_info_sub_ = nullptr;
        as2_msgs::msg::ControllerControlMode BasicMotionCommandsHandler::current_mode_ =
            as2_msgs::msg::ControllerControlMode();

    } // namespace motionCommandsHandlers
} // namespace as2