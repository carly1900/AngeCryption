require 'test_helper'

class EncryptionsControllerTest < ActionDispatch::IntegrationTest
  test "should get index" do
    get encryptions_index_url
    assert_response :success
  end

  test "should get new" do
    get encryptions_new_url
    assert_response :success
  end

  test "should get create" do
    get encryptions_create_url
    assert_response :success
  end

  test "should get destroy" do
    get encryptions_destroy_url
    assert_response :success
  end

end
