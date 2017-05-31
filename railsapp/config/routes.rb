Rails.application.routes.draw do
  scope(:path => '/tmp') do
    resources :encryptions, only: [:index, :new, :create, :destroy]
    root "encryptions#index"
  end
end
