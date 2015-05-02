/*
 * Copyright (C) 2014 Luke San Antonio
 * All rights reserved.
 */
#pragma once
#include <vector>
#include <memory>

#include "element.h"
namespace game { namespace ui
{
  struct null_layout_t {};

  template <class Layout_Type>
  struct View_Child
  {
    Shared_Element view;
    Layout_Type layout;
  };

  template <class Layout_Type>
  struct Element_Composite : public Element
  {
    virtual ~Element_Composite() {}

    virtual bool is_container() const noexcept { return true; }

    using child_t = View_Child<Layout_Type>;
    using child_vec_t = std::vector<child_t>;

    inline void push_child(Shared_Element,
                           Layout_Type = Layout_Type{}) noexcept;
    inline void push_child(child_t) noexcept;

    inline void insert_child(typename child_vec_t::iterator,
                             Shared_Element,
                             Layout_Type = Layout_Type{}) noexcept;
    inline void insert_child(typename child_vec_t::const_iterator,
                             Shared_Element,
                             Layout_Type = Layout_Type{}) noexcept;
    inline void insert_child(typename child_vec_t::iterator,
                             child_t) noexcept;
    inline void insert_child(typename child_vec_t::const_iterator,
                             child_t) noexcept;

    inline void remove_child(Shared_Element) noexcept;
    inline void remove_child(typename child_vec_t::size_type) noexcept;
    inline void remove_child(typename child_vec_t::iterator) noexcept;
    inline void remove_child(typename child_vec_t::const_iterator) noexcept;

    // We don't want any of that single child crap, so just make these
    // functions no-ops for a while.
    void set_child(std::weak_ptr<Element>) noexcept override {}
    inline void remove_child() noexcept override {}
  protected:
    child_vec_t children_;

  private:
    Shared_Element find_child_(std::string, bool) const noexcept override;
    bool replace_child_(std::string, Shared_Element, bool) noexcept override;

    // Left to be implemented
    virtual Volume<int> layout_() = 0;
    virtual Vec<int> get_minimum_extents_() const noexcept = 0;

    void render_(Renderer&) const noexcept override;
    void activate_regions_(Controller&) const noexcept override;
  };

  using Null_Element_Composite = Element_Composite<null_layout_t>;
} }

#include "element_composite_impl.hpp"