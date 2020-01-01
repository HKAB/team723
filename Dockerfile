FROM dungpb/dira_ros:ros
RUN apt-get update && apt-get install -y ros-melodic-image-transport-plugins