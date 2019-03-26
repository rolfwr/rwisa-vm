#pragma once

// utility template function for working with std::variant
// taken from
// https://arne-mertz.de/2018/05/overload-build-a-variant-visitor-on-the-fly/
template<class... Fs>
struct overload : Fs...
{
  template<class... Ts>
  overload(Ts&&... ts)
    : Fs{ std::forward<Ts>(ts) }...
  {}

  using Fs::operator()...;
};

template<class... Ts>
overload(Ts&&...)->overload<std::remove_reference_t<Ts>...>;
