#include "codecs/webp.h"
#include "codecs/file.h"
#include <iostream>

#include <webp/encode.h>
#include <webp/mux.h>
#include <webp/demux.h>

#include <cstdio>

WebP::WebP(std::vector<Image> frames, std::vector<float> delays, unsigned loops)
    : frames(frames), delays(delays), loops(loops) {}

void WebP::save(std::filesystem::path path) {
    Vec size = frames[0].size;

    WebPConfig config;
    config.image_hint = WEBP_HINT_GRAPH;
    config.thread_level = 1;
    WebPConfigInit(&config);
    WebPConfigLosslessPreset(&config, 6);

    WebPAnimEncoderOptions opts;
    WebPAnimEncoderOptionsInit(&opts);
    opts.anim_params.loop_count = loops;

    WebPAnimEncoder *enc = WebPAnimEncoderNew(size.x, size.y, &opts);

    WebPPicture pic;
    WebPPictureInit(&pic);
    pic.width = size.x;
    pic.height = size.y;
    pic.use_argb = true;
    pic.argb_stride = size.x;
    WebPPictureAlloc(&pic);

    float time = 0;
    int index = 0;
    for (auto &frame : frames) {
        // fucking disgusting lmao
        // please SSE work your magic on this
        for (unsigned i = 0; i < size.y; i++) {
            for (unsigned j = 0; j < size.x; j++) {
                unsigned off = i * 4 * size.x + j * 4;
                auto p = &frame.pixels[off];
                pic.argb[i * size.x + j] =
                    p[3] << 24 | p[0] << 16 | p[1] << 8 | p[2];
            }
        }

        WebPAnimEncoderAdd(enc, &pic, time * 100, &config);
        if (delays.size())
            time += delays[index++];
    }

    WebPAnimEncoderAdd(enc, nullptr, time * 100, nullptr);

    WebPData data;
    WebPDataInit(&data);
    WebPAnimEncoderAssemble(enc, &data);
    WebPAnimEncoderDelete(enc);

    FileHandle file{path.string().c_str(), "wb"};
    fwrite(data.bytes, 1, data.size, file.file);

    WebPPictureFree(&pic);
    WebPDataClear(&data);
}

WebP::DecoderHandle::DecoderHandle(WebPData &data, WebPAnimDecoderOptions &opts)
    : d(WebPAnimDecoderNew(&data, &opts))
{}

WebP::DecoderHandle::~DecoderHandle() {
    WebPAnimDecoderDelete(d);
}

void WebP::load(std::filesystem::path path) {
    FileHandle file{path.string().c_str(), "rb"};
    size_t size = std::filesystem::file_size(path);

    WebPData data;
    std::vector<uint8_t> anim_data;
    anim_data.resize(size);

    fread(anim_data.data(), 1, size, file.file);
    data.size = size;
    data.bytes = anim_data.data();

    WebPAnimDecoderOptions opts;
    WebPAnimDecoderOptionsInit(&opts);
    opts.use_threads = 1;

    DecoderHandle dec{data, opts};
    WebPAnimInfo anim_info;
    if (!WebPAnimDecoderGetInfo(dec.d, &anim_info))
        return;

    loops = anim_info.loop_count;

    int prev_time = 0;

    while (WebPAnimDecoderHasMoreFrames(dec.d)) {
        uint8_t *buf;
        int timestamp;
        WebPAnimDecoderGetNext(dec.d, &buf, &timestamp);
        delays.push_back((timestamp-prev_time) / 100.0);
        prev_time = timestamp;
        auto &f = frames.emplace_back();
        f.pixels.reserve(anim_info.canvas_height * anim_info.canvas_width * 4);

        f.size = {anim_info.canvas_width, anim_info.canvas_height};
        for (unsigned i = 0;
             i < anim_info.canvas_height * anim_info.canvas_height; i++) {
            f.pixels[i*4] = buf[i] >> 16;
            f.pixels[i*4+1] = buf[i] >> 8;
            f.pixels[i*4+2] = buf[i];
            f.pixels[i*4+3] = buf[i] >> 24;
        }
    }
}
