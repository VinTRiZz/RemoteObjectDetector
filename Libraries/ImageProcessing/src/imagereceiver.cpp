// #include "imagereceiver.hpp"
// #include <gst/gst.h>
// #include <gst/app/gstappsink.h>
// #include <unordered_map>
// #include <opencv2/core.hpp>
// #include <opencv2/imgproc.hpp>

// namespace ImageProcessing {

// class GStreamerHandler {
// public:
//     using FrameCallback = std::function<void(cv::Mat&&)>;

//     GStreamerHandler(oneapi::tbb::task_arena& arena, uint16_t listenPort)
//         : arena_(arena), listenPort_(listenPort) {}

//     void setCameraConfig(const std::string& cameraId, const Utility::CameraPipelineConfig& config);
//     bool start();
//     void stop();
//     bool isRunning() const { return running_; }

// private:
//     struct Camera {
//         GstElement* pipeline = nullptr;
//         FrameCallback callback;
//     };

//     static GstFlowReturn newPreroll(GstAppSink* sink, gpointer data);
//     static GstFlowReturn newSample(GstAppSink* sink, gpointer data);

//     void handleFrame(const std::string& cameraId, cv::Mat&& frame);

//     oneapi::tbb::task_arena& arena_;
//     uint16_t listenPort_;
//     bool running_ = false;
//     std::unordered_map<std::string, Camera> cameras_;
// };

// struct ImageReceiver::Impl {
//     GStreamerHandler gstreamerHandler;

//     Impl(oneapi::tbb::task_arena& arena, uint16_t listenPort)
//         : gstreamerHandler(arena, listenPort) {}

//     void setCameraConfig(const std::string& cameraId, const Utility::CameraPipelineConfig& config) {
//         gstreamerHandler.setCameraConfig(cameraId, config);
//     }

//     void setFrameCallback(const std::string& cameraId, GStreamerHandler::FrameCallback callback) {
//         cameras_[cameraId] = callback;
//     }

//     bool start() { return gstreamerHandler.start(); }
//     void stop() { gstreamerHandler.stop(); }
//     bool isRunning() const { return gstreamerHandler.isRunning(); }

// private:
//     std::unordered_map<std::string, GStreamerHandler::FrameCallback> cameras_;
// };

// ImageReceiver::ImageReceiver(oneapi::tbb::task_arena& arena, uint16_t listenPort)
//     : d(std::make_unique<Impl>(arena, listenPort)) {}

// ImageReceiver::~ImageReceiver() {
//     stop();
// }

// void ImageReceiver::setFrameCallback(const std::string& cameraId, std::function<void(cv::Mat&&)> callback) {
//     d->setFrameCallback(cameraId, std::move(callback));
// }

// void ImageReceiver::setCameraConfig(const std::string& cameraId, const Utility::CameraPipelineConfig& conf) {
//     d->setCameraConfig(cameraId, conf);
// }

// bool ImageReceiver::start() { return d->start(); }
// void ImageReceiver::stop() { d->stop(); }
// bool ImageReceiver::isRunning() const { return d->isRunning(); }

// // GStreamerHandler implementation

// void GStreamerHandler::setCameraConfig(const std::string& cameraId, const Utility::CameraPipelineConfig& config) {
//     if (cameras_.find(cameraId) != cameras_.end()) {
//         // Destroy old pipeline
//         GstElement* pipeline = cameras_[cameraId].pipeline;
//         if (pipeline) {
//             gst_element_set_state(pipeline, GST_STATE_NULL);
//             gst_object_unref(GST_OBJECT(pipeline));
//         }
//     }

//     std::string pipelineDesc = Utility::createReceiverPipeline(config);
//     cameras_[cameraId] = {gst_parse_launch(pipelineDesc.c_str(), nullptr), nullptr};
// }

// bool GStreamerHandler::start() {
//     running_ = true;
//     for (auto& [cameraId, camera] : cameras_) {
//         GstBus* bus = gst_element_get_bus(camera.pipeline);
//         gst_bus_add_signal_watch(bus);

//         g_signal_connect(G_OBJECT(bus), "message", G_CALLBACK(+[](GstBus*, GstMessage* msg, gpointer data) ->
//                                                               gboolean {
//                                                                   GStreamerHandler* self = static_cast<GStreamerHandler*>(data);
//                                                                   if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
//                                                                       // TODO: Process error
//                                                                   } else if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
//                                                                       GError* err;
//                                                                       gchar* debug;

//                                                                       gst_message_parse_error(msg, &err, &debug);
//                                                                       g_printerr("Error: %s\n", err->message);
//                                                                       g_clear_error(&err);
//                                                                       g_free(debug);

//                                                                       self->stop();
//                                                                   }
//                                                                   return TRUE;
//                                                               }), this);

//         gst_object_unref(bus);

//         GstAppSink* appsink = GST_APP_SINK(gst_bin_get_by_name(GST_BIN(camera.pipeline), "appsink"));
//         gst_app_sink_set_emit_signals(appsink, true);
//         g_signal_connect(G_OBJECT(appsink), "new-sample", G_CALLBACK(newSample), this);
//         gst_object_unref(appsink);

//         gst_element_set_state(camera.pipeline, GST_STATE_PLAYING);
//     }
//     return running_;
// }

// void GStreamerHandler::stop() {
//     running_ = false;
//     for (auto& [cameraId, camera] : cameras_) {
//         if (camera.pipeline) {
//             gst_element_set_state(camera.pipeline, GST_STATE_NULL);
//             gst_object_unref(GST_OBJECT(camera.pipeline));
//             camera.pipeline = nullptr;
//         }
//     }
// }

// GstFlowReturn GStreamerHandler::newSample(GstAppSink* sink, gpointer data) {
//     GStreamerHandler* self = static_cast<GStreamerHandler*>(data);

//     GstSample* sample = gst_app_sink_pull_sample(sink);
//     if (!sample)
//         return GST_FLOW_ERROR;

//     GstBuffer* buffer = gst_sample_get_buffer(sample);
//     GstMapInfo map;
//     gst_buffer_map(buffer, &map, GST_MAP_READ);

//     // Assuming RGB format for simplicity
//     // cv::Mat frame(map.height, map.width, CV_8UC3, (char*)map.data);
//     // cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB); // Convert to BGR if necessary

//     // std::string cameraId = "your_camera_id"; // Parse camera ID from the stream or use some other method
//     // self->handleFrame(cameraId, std::move(frame));

//     gst_buffer_unmap(buffer, &map);
//     gst_sample_unref(sample);

//     return GST_FLOW_OK;
// }

// void GStreamerHandler::handleFrame(const std::string& cameraId, cv::Mat&& frame) {
//     auto it = cameras_.find(cameraId);
//     if (it != cameras_.end() && it->second.callback) {
//         arena_.enqueue([callback = it->second.callback, frame = std::move(frame)]() mutable {
//             callback(std::move(frame));
//         });
//     }
// }

// } // namespace ImageProcessing