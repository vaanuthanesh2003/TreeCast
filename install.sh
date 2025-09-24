curl https://pyenv.run | bash
export PYENV_ROOT="$HOME/.pyenv"
export PATH="$PYENV_ROOT/bin:$PATH"
eval "$(pyenv init -)"
eval "$(pyenv virtualenv-init -)"
pyenv install 3.10.14
pyenv global 3.10.14
python -m venv venv
source venv/bin/activate
pip install gdown
gdown 1tHWbX-TdkS4JCg6MrUu-BAUDjPDTaMyQ
wget https://huggingface.co/HCMUE-Research/SAM-vit-h/resolve/main/sam_vit_h_4b8939.pth
pip install build
pip install cmake
pip install wheel
pip install ninja
pip install torch==2.5.1 torchvision torchaudio --index-url https://download.pytorch.org/whl/cu121
pip install -r Magic123/requirements.txt --no-build-isolation
pip install -r zero123/zero123/requirements.txt --no-build-isolation
