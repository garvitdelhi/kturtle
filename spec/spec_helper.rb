#  Copyright (C) 2009 by Cies Breijs
#  Copyright (C) 2009 by Niels Slot
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public
#  License as published by the Free Software Foundation; either
#  version 2 of the License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public
#  License along with this program; if not, write to the Free
#  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#  Boston, MA 02110-1301, USA.

require 'rubygems'
require 'singleton'

begin
  require 'Qt'
rescue LoadError
  begin
    require 'rbus'
  rescue LoadError
    puts "Couldn't load either rdbus or Qt, please install one, quiting now.."
    exit
  end
end

class Interpreter
  include Singleton

  def connect
    @pid = ENV['KTURTLE_INTERPRETER_DBUS_PID'] || IO.readlines("pid")[0].to_i
    puts "Opening DBus connection with KTurtle (pid: #{@pid})..."

    # Use Qt DBus if it's found, otherwise fall back to rbus
    if Object.const_defined?(:Qt)
      @interpreter = Qt::DBusInterface.new("org.kde.kturtle-#{@pid}", '/Interpreter', 'org.kde.kturtle.Interpreter')
    else
      @interpreter = RBus.session_bus.get_object("org.kde.kturtle-#{@pid}", '/Interpreter')
      @interpreter.interface!('org.kde.kturtle.Interpreter')
    end
  end

  def load(code); connect unless @pid; @interpreter.initialize code; nil; end
  def interpret;  @interpreter.interpret; nil; end
  def errors?;    @interpreter.encounteredErrors; end
  def errors;     @interpreter.getErrorStrings; end
  def state;      [:uninitialized, :initialized, :parsing, :executing, :finished, :aborted][@interpreter.state]; end
  def inspect;    "#<Interpreter pid:#{@pid}>"; end

  def run(code)
    load code
    while not [:finished, :aborted].include? state
      interpret
    end
    self  # return self for easy method stacking
  end

  def should_run_clean(code)
    run(code)
    errors?.should == false
    p errors if errors?
  end
end