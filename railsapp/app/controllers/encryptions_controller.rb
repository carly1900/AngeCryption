#import 'ffi'

module AngeCryption
  extend FFI::Library
  ffi_lib 'angecryption'
  attach_function :angecrypt, [ :string, :string, :string, :string, :pointer ], :int
end

class EncryptionsController < ApplicationController
  def index
    @encryptions = Encryption.all
  end

  def new
    @encryption = Encryption.new
  end

  def create
    @encryption = Encryption.new
    @encryption.source = params[:encryption][:source]
    @encryption.fake = params[:encryption][:fake]
    password = params[:password][0,16]
    source_path = params[:encryption][:source].path
    fake_path = params[:encryption][:fake].path
    result_path = "#{File.dirname(source_path)}/#{File.basename(source_path, '.*')}#{File.basename(fake_path)}"
    ivptr = FFI::MemoryPointer.new(:char, 16)

    AngeCryption.angecrypt(source_path, fake_path, password, result_path, ivptr)
    @encryption.iv = ivptr.read_string_length(16).each_byte.map { |b| b.to_s(16) }.join
    
    File.open(result_path) do |f|
      @encryption.encrypted = f
    end

    if @encryption.save
      redirect_to encryptions_path, notice: "Uploaded successfully"
    else
      render "new"
    end
  end

  def destroy
    @encryption = Encryption.find(params[:id])
    @encryption.destroy
  end
end
