{
  'targets': [
    {
      'target_name': 'pHash',
      'sources': [ 'phash.cpp' ],
      'cflags!': [
        '-fno-exceptions',
        '<!@(pkg-config --cflags pHash)'
      ],
      'cflags_cc!': [ '-fno-exceptions' ],
      'ldflags' : [
        '<!@(pkg-config --libs-only-L --libs-only-other pHash)'
      ],
      'libraries' : [
        '<!@(pkg-config --libs-only-l pHash)'
      ],
      'conditions': [
        ['OS=="mac"', {
          'xcode_settings': {
            'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
            'OTHER_CFLAGS' : [
              '-mmacosx-version-min=10.7',
              '<!@(pkg-config --cflags pHash)',
              '-std=c++11',
              '-stdlib=libc++'
            ]
          }
        }]
      ],
      'link_settings': {
        'libraries': ['<!@(pkg-config --libs pHash)']
      },
      "include_dirs" : [
        "<!(node -e \"require('nan')\")",
        "/usr/local/include"
      ]
    }
  ]
}
