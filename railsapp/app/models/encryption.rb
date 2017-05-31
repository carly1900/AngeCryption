class Encryption < ApplicationRecord
  mount_uploader :fake, AttachmentUploader
  mount_uploader :source, AttachmentUploader
  mount_uploader :encrypted, AttachmentUploader
end
