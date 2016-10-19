// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once
#ifndef INCLUDE_TCP_FD_HPP
#define INCLUDE_TCP_FD_HPP

#include "fd.hpp"
#include <net/inet4>
#include <ringbuffer>

struct TCP_FD_Conn;
struct TCP_FD_Listen;

class TCP_FD : public FD {
public:
  using id_t = int;

  explicit TCP_FD(int id)
    : FD(id)
  {}

  int     read(void*, size_t) override;
  int     write(const void*, size_t) override;
  int     close() override;
  /** SOCKET */
  int     accept(struct sockaddr *__restrict__, socklen_t *__restrict__) override;
  int     bind(const struct sockaddr *, socklen_t) override;
  int     connect(const struct sockaddr *, socklen_t) override;

  ssize_t send(const void *, size_t, int fl) override;
  ssize_t recv(void*, size_t, int fl) override;

  bool is_listener() const noexcept {
    return ld != nullptr;
  }
  bool is_connection() const noexcept {
    return cd != nullptr;
  }

  ~TCP_FD() {}
private:
  TCP_FD_Conn* cd = nullptr;
  TCP_FD_Listen*  ld = nullptr;
  // sock opts
  bool non_blocking = false;
  
  friend struct TCP_FD_Listen;
};

struct TCP_FD_Conn
{
  TCP_FD_Conn(net::tcp::Connection_ptr c)
    : conn(c), readq(16484)
  {}
  
  void recv_to_ringbuffer(net::tcp::buffer_t, size_t);
  void set_default_read();
  
  ssize_t send(const void *, size_t, int fl);
  ssize_t recv(void*, size_t, int fl);
  int     close();
  
  net::tcp::Connection_ptr conn;
  RingBuffer readq;
};

struct TCP_FD_Listen
{
  TCP_FD_Listen(net::tcp::Listener& l)
    : listener(l)
  {}
  
  int close();
  int listen(int);
  int accept(struct sockaddr *__restrict__, socklen_t *__restrict__);
  
  net::tcp::Listener& listener;
  std::deque<net::tcp::Connection_ptr> connq;
};

#endif
