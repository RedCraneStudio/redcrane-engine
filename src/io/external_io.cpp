/*
 * Copyright (C) 2015 Luke San Antonio
 * All rights reserved.
 */
#include "external_io.h"
#include "common.h"
#include "../common/utility.h"
namespace redc
{
  void post_pipe_buffer(ipc::Pipe* p) noexcept
  {
    External_IO* io = (External_IO*) p->user_data;
    io->post(*p->buf);
  }
  void post_error_from_buffer(ipc::Pipe* p) noexcept
  {
    External_IO* io = (External_IO*) p->user_data;
    io->post_error(*p->buf);
  }

  Child_Process::Child_Process(ipc::Spawn_Options& opt) noexcept
  {
    uv_loop_init(&loop_);

    process_ = ipc::create_process(&loop_, opt);

    process_->io.in.user_data = this;
    process_->io.in.action_cb = post_pipe_buffer;
    uv_read_start((uv_stream_t*) &process_->io.in, alloc, ipc::collect_lines);

    process_->err.user_data = this;
    process_->err.action_cb = post_error_from_buffer;
    uv_read_start((uv_stream_t*) &process_->err, alloc, ipc::collect_lines);

    // Start the boll rollin'!
    //*process_->io.out.buf = buf_from_string("PpM");
    ipc::write_buffer(&process_->io.out);
  }
  Child_Process::~Child_Process() noexcept
  {
    ipc::delete_process(process_);

    uv_run(&loop_, UV_RUN_DEFAULT);
    uv_loop_close(&loop_);
  }
  void Child_Process::write(buf_t const& buf) noexcept
  {
    step();
    if(!process_->running) return;
    *process_->io.out.buf = buf;
    ipc::write_buffer(&process_->io.out);
  }
  void Child_Process::step() noexcept
  {
    uv_run(&loop_, UV_RUN_NOWAIT);
  }

  Pipe_IO::Pipe_IO() noexcept
    : External_IO(), cp_(make_owned_maybe(new Pipe_IO(*this))) {}

  Pipe_IO::Pipe_IO(Pipe_IO& io) noexcept
    : External_IO(), cp_(&io, false) {}

  Pipe_IO& Pipe_IO::counterpart() noexcept
  {
    return *cp_;
  }

  void Pipe_IO::write(buf_t const& buf) noexcept
  {
    // Write to the counterpart's input.
    counterpart().input_.push(buf);
  }
  void Pipe_IO::step() noexcept
  {
    step_(true);
  }
  void Pipe_IO::step_(bool recursive) noexcept
  {
    // Read our pending input.
    while(!input_.empty())
    {
      buf_t const& buf = input_.front();
      post(buf);
      input_.pop();
    }

    // Let our counterpart do the same.
    if(recursive)
    {
      counterpart().step_(false);
    }
  }
}
