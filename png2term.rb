# coding: utf-8
require 'oily_png'

$tab = 216.times.map do |i|
  ('%03d' % i.to_s(6)).split('').map { |j| 55 * j.to_i }
end

def term r, g, b, a
  16 + $tab.index($tab.min_by { |x, y, z| [r - x, g - y, z - b].map &:abs })
end

def rgb color
  (0..3).map { |i| (color >> i * 8) & 255 }.reverse
end

sprite = ChunkyPNG::Image.from_stream ARGF

(0...sprite.height).step(2).each do |y|
  top = sprite.row(y).map     { |p| p == 0 ? 0 : term(*rgb(p)) }
  bot = sprite.row(y + 1).map { |p| p == 0 ? 0 : term(*rgb(p)) } rescue [0].cycle
  top.zip(bot).each do |t, b|
    if t == 0 && b == 0
      print "\e[0m "
    elsif t == 0
      print "\e[0m\e[38;5;#{b}m▄"
    elsif b == 0
      print "\e[0m\e[38;5;#{t}m▀"
    else
      print "\e[48;5;#{b}m\e[38;5;#{t}m▀"
    end
  end
  puts "\e[0m"
end
